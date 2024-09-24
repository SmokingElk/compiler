#include "binCodeGen.h"
#include "mappersDeclaration.h"

#define __DEBUG_BIN_CODE_GEN__ 

typedef Operand (*opBinCodeMapper)(BinCodeGen, List, Map);

void _addOpMappers (BinCodeGen gen);
bool _mapNumber (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, Map typeTable);
bool _mapName (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, PriorityNameTable nameTable, Map typeTable);
bool _mapFunction (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, PriorityNameTable nameTable, Map typeTable);
bool _mapOperator (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, Map typeTable, SyntaxAnalyzer analyzer);

BinCodeGen createBinCodeGen () {
    BinCodeGen gen = (BinCodeGen)malloc(sizeof(_BinCodeGen));

    gen->instrWritten = 0;
    gen->error = NULL;
    gen->out = NULL;

    gen->freeRegs = createStack(0);
    gen->opMappers = createMap(0);
    gen->funcsAddrs = createMap(0);
    gen->linkSequence = createLinkSequence();

    gen->prepatchedFile = fopen(PREPATCHED_FILE_PATH, "w+");

    _addOpMappers(gen);
    resetBinCodeGen(gen);

    return gen;
}

BinCodeGen setOut (BinCodeGen gen, FILE *file) {
    gen->out = file;
}

void deleteBinCodeGen (BinCodeGen gen) {
    while (!isEmptyStack(gen->freeRegs)) {
        free(popStack(LOCATION, gen->freeRegs));
    }
    
    deleteStack(gen->freeRegs);
    deleteMap(gen->opMappers, mapDestructorPlug);
    deleteMap(gen->funcsAddrs, addrDestructor);
    deleteLinkSequence(gen->linkSequence);
    
    fclose(gen->prepatchedFile);
    free(gen);
}

void resetBinCodeGen (BinCodeGen gen) {
    while (!isEmptyStack(gen->freeRegs)) {
        free(popStack(LOCATION, gen->freeRegs));
    }

    for (int i = REG_COMMON_END; i >= REG_COMMON_START; i--) {
        pushStack(gen->freeRegs, createLoc(i));
    }
}

void generatePrefixCode (BinCodeGen gen) {
    instruction instr;

    // переместить начало стека в середину памяти
    instr = i_WRITE(REG_BP, RAM_CAPACITY / 2 - 1);
    writeInstruction(gen, instr);

    // записываем адрес возврата из точки входа (сразу за BP)
    LOCATION locForRetAddr = allocReg(gen);

    instr = i_WRITE(getReg(locForRetAddr), shiftInstrAddr(gen->instrWritten, 3)); 
    writeInstruction(gen, instr);

    freeReg(gen, locForRetAddr);

    instr = i_SAVER(1, MEM_WORD_16, getReg(locForRetAddr));
    writeInstruction(gen, instr);

    // прыжок на точку входа
    instr = i_JUMP(0);
    writeInstruction(gen, instr);

    // добавляем ссылку на точку входа, которую надо будет закрыть
    addBacklinkSequence(gen->linkSequence, gen->instrWritten);

    // поднятие флага завершения
    instr = i_ALUC(ALU_OR, REG_RS, 1 << (sizeof(word) * 8 - 1));
    writeInstruction(gen, instr);
}

CompileError genErrorBinCodeGen (BinCodeGen gen) {
    return gen->error;
}

REGISTER postfixToBinCode (BinCodeGen gen, Queue in, PriorityNameTable nameTable, Map typeTable, SyntaxAnalyzer analyzer, bool *res) {
    *res = true;
    Stack operandsStack = createStack(0);

    #ifdef __DEBUG_BIN_CODE_GEN__
        printf("Instructions:\n");
        printf("ADDR\tOP\tALU\tMEM_M\tR1\tR2\tIMM\n");
    #endif

    while (!isEmptyQueue(in)) {
        Lexeme topLexeme = popQueue(Lexeme, in);
        LexType topLexType = typeLexeme(topLexeme);

        switch (topLexType) {
            case LEX_NUMBER:
                *res = _mapNumber(gen, topLexeme, operandsStack, typeTable);
                break;
            
            case LEX_NAME:
                if (hasMap(gen->funcsAddrs, bufferString(contentLexeme(topLexeme)))) {
                    *res = _mapFunction(gen, topLexeme, operandsStack, nameTable, typeTable);
                } else {
                    *res = _mapName(gen, topLexeme, operandsStack, nameTable, typeTable);
                }
                break;

            case LEX_OPERATOR:
                *res = _mapOperator(gen, topLexeme, operandsStack, typeTable, analyzer);
                break;

            default:
                break;
        }
        
        deleteLexeme(topLexeme);
        
        if (!(*res)) break;
    }

    REGISTER resultReg = REG_RS; 
    if (!isEmptyStack(operandsStack)) resultReg = getReg(valueLocOperand(topStack(Operand, operandsStack)));

    while (!isEmptyStack(operandsStack)) {
        freeReg(gen, valueLocOperand(topStack(Operand, operandsStack)));
        deleteOperand(popStack(Operand, operandsStack));
    }

    deleteStack(operandsStack);
    return resultReg;
}

void addBranchBinCodeGen (BinCodeGen gen, REGISTER conditionValueReg) {
    instruction instr;

    // сравнить значение условия с нулем (false)
    instr = i_CMPC(conditionValueReg, 0);
    writeInstruction(gen, instr);

    // создаем заготовку инструкции перехода
    // адрес 0 заменяется реальным на этапе backpatching
    instr = i_BCON(CMP_EQ, 0);
    writeInstruction(gen, instr);

    // добавить не закрытую ссылку
    addBacklinkSequence(gen->linkSequence, gen->instrWritten);
}

void closeBranch (BinCodeGen gen) {
    // запомнить адрес
    closeBacklinkSequence(gen->linkSequence, gen->instrWritten);
}

void addLoopLinkBinCodeGen (BinCodeGen gen) {
    addBacklinkSequence(gen->linkSequence, gen->instrWritten);
}

void addLoopConditionBinCodeGen (BinCodeGen gen, REGISTER conditionValueReg) {
    instruction instr;

    instr = i_CMPC(conditionValueReg, 0);
    writeInstruction(gen, instr);

    instr = i_BCON(CMP_EQ, 0); 
    writeInstruction(gen, instr);
}

void closeLoop (BinCodeGen gen) {
    word addrFrom = getCurrentAddrFromSequence(gen->linkSequence);
    printf("addrFrom: %d\n", addrFrom);

    instruction instr;

    // прыжок в условие в конце цикла
    instr = i_JUMP(addrFrom);
    writeInstruction(gen, instr);

    closeBacklinkSequence(gen->linkSequence, gen->instrWritten);
}

void backpatchLinks (BinCodeGen gen) {
    instruction instr;

    #ifdef __DEBUG_BIN_CODE_GEN__
        printf("\nBackpatching:\n");
    #endif

    fseek(gen->prepatchedFile, 0, SEEK_SET);

    setBeginSequence(gen->linkSequence);

    word curInstrAddr = 0;

    while (!feof(gen->prepatchedFile)) {
        fread(&instr, sizeof(instruction), 1, gen->prepatchedFile);

        OP_CODE instrOp = getOpCode(instr);

        if ((instrOp == OP_JUMP || instrOp == OP_BCON) && getImmediate(instr) == 0) {
            // нашли ссылку для backpatch
            word addr = nextAddrToSequence(gen->linkSequence);
            instr = buildInstr(instrOp, (ALU_MODE)getCmpMode(instr), MEM_WORD_16, REG_RS, REG_RS, addr);
            
            #ifdef __DEBUG_BIN_CODE_GEN__
                printf("Backpatch. Instruction: %d, Jump addr: %d\n", curInstrAddr * sizeof(instruction), addr);
            #endif 
        } 

        instr = reverseBytesInstruction(instr);

        fwrite(&instr, sizeof(instruction), 1, gen->out);
        curInstrAddr++;
    }
}

void functionHereBinCodeGen (BinCodeGen gen, char name[]) {
    setMap(gen->funcsAddrs, name, createAddr(gen->instrWritten));

    if (strcmp(name, ENTRY_NAME) == 0) {
        // заполняем ссылку на точку входа, она всегда первая
        closeFirstBackinkSequence(gen->linkSequence, gen->instrWritten);
    }
}

void generateReturnBinCodeGen (BinCodeGen gen, Map typeTable, string funcName, REGISTER resReg) {
    instruction instr;
    
    MemoryStack localStack = createMemoryStack(0);

    // является ли текущая функция точкой входа
    bool isEntry = strcmp(bufferString(funcName), ENTRY_NAME) == 0;

    if (!isEntry) {
        // получить тип возвращаемого значения
        Type funcType = getMap(Type, typeTable, bufferString(funcName));
        Type retType = getMap(Type, typeTable, bufferString(iterFetch(string, componentFirstType(funcType))));

        // поместить возвращаемое значение на стек
        instr = pushMemoryStack(localStack, memModeType(retType), resReg);
        writeInstruction(gen, instr);

        // фиктивное удаление со стека (значение будет извлечено после перехода на адрес возврата, см. _mapFunction)
        popMemoryStack(localStack, memModeType(retType), resReg);
    }

    // прочитать адрес возврата
    instr = popMemoryStack(localStack, MEM_WORD_16, REG_RA);
    writeInstruction(gen, instr);

    if (!isEntry) {
        // возвращаем регистрам значения при вызове
        for (int i = REG_COMMON_END; i >= REG_COMMON_START; i--) {
            instr = popMemoryStack(localStack, MEM_WORD_16, i);
            writeInstruction(gen, instr);
        } 

        // получаем значение BP
        instr = popMemoryStack(localStack, MEM_WORD_16, REG_BP);
        writeInstruction(gen, instr);
    }

    // выполняем переход на адрес возврата
    instr = i_JUMPP(REG_RA);
    writeInstruction(gen, instr);

    deleteMemoryStack(localStack);
}

LOCATION allocReg (BinCodeGen gen) {
    return popStack(LOCATION, gen->freeRegs);
}

void freeReg (BinCodeGen gen, LOCATION loc) {
    pushStack(gen->freeRegs, loc);
}

void freeOperandReg (BinCodeGen gen, Operand operand) {
    freeReg(gen, valueLocOperand(operand));
}

void writeInstruction (BinCodeGen gen, instruction instr) {
    #ifdef __DEBUG_BIN_CODE_GEN__
        printf("%d\t", gen->instrWritten);
        printInstruction(instr);
        printf("\n");
    #endif

    fwrite(&instr, sizeof(instruction), 1, gen->prepatchedFile);
    gen->instrWritten = shiftInstrAddr(gen->instrWritten, 1);
}

void _addOpMappers (BinCodeGen gen) {
    setMap(gen->opMappers, OPERATOR_ASSIGNMENT, mapperAssigment);
    setMap(gen->opMappers, OPERATOR_PLUS, mapperPlus);
    setMap(gen->opMappers, OPERATOR_MINUS, mapperMinus);
    setMap(gen->opMappers, OPERATOR_PRINT, mapperPrint);
    setMap(gen->opMappers, OPERATOR_EQUAL, mapperEqual);
    setMap(gen->opMappers, OPERATOR_NOT_EQUAL, mapperNotEqual);
    setMap(gen->opMappers, OPERATOR_LESS, mapperLess);
    setMap(gen->opMappers, OPERATOR_LESS_EQUAL, mapperLessEqual);
    setMap(gen->opMappers, OPERATOR_MORE, mapperMore);
    setMap(gen->opMappers, OPERATOR_MORE_EQUAL, mapperMoreEqual);
    setMap(gen->opMappers, OPERATOR_OR, mapperOr);
    setMap(gen->opMappers, OPERATOR_AND, mapperAnd);
    setMap(gen->opMappers, OPERATOR_NOT, mapperNot);
    setMap(gen->opMappers, OPERATOR_BITWISE_OR, mapperBitwiseOr);
}

bool _mapNumber (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, Map typeTable) {
    LOCATION location = allocReg(gen);
    instruction instr = i_WRITE(getReg(location), readNumLexeme(lexeme)); 
    writeInstruction(gen, instr);

    pushStack(operandsStack, createOperand(location, 0, getMap(Type, typeTable, NUM_LITERAL_TYPE)));
    return true;
}

bool _mapName (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, PriorityNameTable nameTable, Map typeTable) {
    LOCATION location = allocReg(gen);
    char *nameStr = bufferString(contentLexeme(lexeme));
    
    if (!nameInPriorityTable(nameTable, nameStr)) {
        gen->error = (CompileError)createNameError(
            posLexeme(lexeme),
            contentLexeme(lexeme),
            NAME_ERROR_UNDECLARATED
        );

        return false;
    }

    Name name = getNameFromPriorityTable(nameTable, nameStr);
    Type type = getMap(Type, typeTable, typeOfName(name));
    
    instruction instr = i_LOAD(getReg(location), relativeAddrName(name), memModeType(type)); 
    writeInstruction(gen, instr);

    pushStack(operandsStack, createOperand(location, relativeAddrName(name), type));
    return true;
}

bool _mapFunction (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, PriorityNameTable nameTable, Map typeTable) {
    instruction instr;

    // сохранение BP  
    MemoryStack currentStack = createMemoryStack((word)getTopPriorityTable(nameTable)); 

    instr = pushMemoryStack(currentStack, MEM_WORD_16, REG_BP);
    writeInstruction(gen, instr);

    // сохранение регистров
    for (int i = REG_COMMON_START; i < REG_COMMON_END + 1; i++) {
        instr = pushMemoryStack(currentStack, MEM_WORD_16, i);
        writeInstruction(gen, instr);
    }

    // стек для реверса операндов
    int argsCount = 0;
    Stack revOperands = createStack(0);
    
    Type funcType = getMap(Type, typeTable, bufferString(contentLexeme(lexeme)));

    // получить итератор на первый аргумент. Первый компонент в типе функции - возвращаемое значение
    for (ListIter argIt = iterNext(componentFirstType(funcType)); !isLastComponentType(funcType, argIt); argIt = iterNext(argIt)) {
        if (isEmptyStack(operandsStack)) {
            // ошибка: недостаточно аргументов для вызова функции
            // валидация
        }

        pushStack(revOperands, popStack(Operand, operandsStack));
        argsCount++;
    }

    // сохранить адрес возврата
    // текущая инструкция + 1 на сохранение locForRetAddr + 1 на изменение BP + 1 на вызов + по одной на аргумент + 1 на следующую
    word retAddr = shiftInstrAddr(gen->instrWritten, argsCount + 4);

    LOCATION locForRetAddr = allocReg(gen);

    instr = i_WRITE(getReg(locForRetAddr), retAddr);
    writeInstruction(gen, instr);

    instr = pushMemoryStack(currentStack, MEM_WORD_16, getReg(locForRetAddr));
    writeInstruction(gen, instr);

    freeReg(gen, locForRetAddr);

    // передача аргументов
    instr = i_ALUC(ALU_ADD, REG_BP, getTopMemoryStack(currentStack));
    writeInstruction(gen, instr);

    MemoryStack localStack = createMemoryStack(0);

    for (ListIter argIt = iterNext(componentFirstType(funcType)); !isLastComponentType(funcType, argIt); argIt = iterNext(argIt)) {
        Type argType = getMap(Type, typeTable, bufferString(iterFetch(string, argIt)));
        Operand operand = popStack(Operand, revOperands);
        REGISTER argValueReg = valueRegOperand(operand);

        instr = pushMemoryStack(localStack, memModeType(argType), argValueReg);
        writeInstruction(gen, instr);

        freeReg(gen, valueLocOperand(operand));
        deleteOperand(operand);
    }

    // вызов функции
    word funcAddr = readAddr(getMap(ADDR, gen->funcsAddrs, bufferString(contentLexeme(lexeme))));
    instr = i_JUMP(funcAddr);
    writeInstruction(gen, instr);

    // получить возвращенное значение на регистр
    Type retType = getMap(Type, typeTable, bufferString(iterFetch(string, componentFirstType(funcType))));
    LOCATION retLoc = allocReg(gen);

    // синхронизируем со стеком из return (фиктивное добавление, возвращенное значение уже на стеке)
    pushMemoryStack(currentStack, memModeType(retType), getReg(retLoc));
    instr = popMemoryStack(currentStack, memModeType(retType), getReg(retLoc));
    writeInstruction(gen, instr);

    pushStack(operandsStack, createOperand(retLoc, 0, retType));

    deleteStack(revOperands);
    deleteMemoryStack(currentStack);
    deleteMemoryStack(localStack);

    return true;
}

bool _mapOperator (BinCodeGen gen, Lexeme lexeme, Stack operandsStack, Map typeTable, SyntaxAnalyzer analyzer) {
    List operands = createList();

    OpArity operandsCount = getLexOpArity(analyzer, lexeme);

    for (int i = 0; i < operandsCount; i++) {
        if (isEmptyStack(operandsStack)) {
            gen->error = (CompileError)createSyntaxError(
                posLexeme(lexeme),
                contentLexeme(lexeme),
                SYNTAX_ERROR_TOO_FEW_OPERANDS
            );

            for (ListIter it = beginList(operands); iterNotEquals(it, endList(operands)); it = iterNext(it)) {
                deleteOperand(iterFetch(Operand, it));
            }

            deleteList(operands);        
            return false;
        }
        
        insertToList(operands, endList(operands), popStack(Operand, operandsStack));
    }

    opBinCodeMapper mapper = getMap(opBinCodeMapper, gen->opMappers, bufferString(contentLexeme(lexeme)));
    Operand res = mapper(gen, operands, typeTable);

    if (res != NULL) pushStack(operandsStack, res);

    for (ListIter it = beginList(operands); iterNotEquals(it, endList(operands)); it = iterNext(it)) {
        deleteOperand(iterFetch(Operand, it));
    }

    deleteList(operands);
    return true;
}