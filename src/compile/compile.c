#include "compile.h"

typedef enum {
    SCOPE_BLOCK,
    SCOPE_IF,
    SCOPE_ELSE,
    SCOPE_WHILE,
    SCOPE_FUNCTION,
} ScopeType;

typedef struct {
    Lexeme lexeme;
    ScopeType type;
} _ScopeOpen, *ScopeOpen;

ScopeOpen createScopeOpen (Lexeme lexeme, ScopeType type);
void deleteScopeOpen (ScopeOpen scope);
ScopeType typeScopeOpen (ScopeOpen scope);
Lexeme lexemeScopeOpen (ScopeOpen scope);

bool isLexemeOpensScope (LexType type);

void printLexemesQueue (Queue queue);
void typeDestructor (void *object);

void compileInit (Compiler compiler, FILE *out);
void setUnexpectedTopLexeme (Compiler compiler);
bool registrateName (Compiler compiler);
bool registrateFunction (Compiler compiler);
bool processScopes (Compiler compiler, bool funcStarts);
bool compileBranchCondition (Compiler compiler);
bool compileLoopCondition (Compiler compiler);

Compiler createCompiler () {
    Compiler compiler = (Compiler)malloc(sizeof(_Compiler));
    
    compiler->nameTable = createPriorityNameTable();
    compiler->functions = createStrSet();
    compiler->currentFunction = NULL;
    compiler->typeTable = createMap(0);
    compiler->compileError = NULL;
    compiler->lexemesInfix = createQueue(0);
    compiler->lexemesPostfix = createQueue(0);
    compiler->scopesOpenings = createStack(0);
    compiler->lexer = createLexer();
    compiler->analyzer = createSyntaxAnalyzer();
    compiler->codeGen = createBinCodeGen();

    return compiler;
}

void deleteCompiler (Compiler compiler) {
    deleteLexer(compiler->lexer);
    deleteSyntaxAnalyzer(compiler->analyzer);
    deleteBinCodeGen(compiler->codeGen);

    while (!isEmptyQueue(compiler->lexemesInfix)) {
        deleteLexeme(topQueue(Lexeme, compiler->lexemesInfix));
        popQueue(Lexeme, compiler->lexemesInfix);
    }
    
    deleteQueue(compiler->lexemesInfix);

    while (!isEmptyQueue(compiler->lexemesPostfix)) {
        deleteLexeme(topQueue(Lexeme, compiler->lexemesPostfix));
        popQueue(Lexeme, compiler->lexemesPostfix);
    }
    
    deleteQueue(compiler->lexemesPostfix);

    while (!isEmptyStack(compiler->scopesOpenings)) {
        deleteScopeOpen(topStack(ScopeOpen, compiler->scopesOpenings));
        popStack(Lexeme, compiler->scopesOpenings);
    }
    
    deleteStack(compiler->scopesOpenings);

    deleteMap(compiler->typeTable, typeDestructor);
    deletePriorityNameTable(compiler->nameTable);

    deleteStrSet(compiler->functions);

    if (compiler->currentFunction != NULL) deleteString(compiler->currentFunction);

    if (compiler->compileError != NULL) free(compiler->compileError);
    free(compiler);
}

CompileError getCompileError (Compiler compiler) {
    return compiler->compileError;
}

void compileInit (Compiler compiler, FILE *out) {
    compiler->compileError = NULL;

    setMap(compiler->typeTable, BASE_TYPE_INT8, createComponentlessType(1));
    setMap(compiler->typeTable, BASE_TYPE_INT16, createComponentlessType(2));
    setMap(compiler->typeTable, BASE_TYPE_BOOL, createComponentlessType(1));

    addBaseSectionToPriorityTable(compiler->nameTable);
    setOut(compiler->codeGen, out);
}

bool compile (Compiler compiler, FILE *in, FILE *out) {
    compileInit(compiler, out);

    generatePrefixCode(compiler->codeGen);

    do {
        bool lexemesExtractSuccess = getLexemes(compiler->lexer, in, compiler->lexemesInfix);

        if (!lexemesExtractSuccess) {
            compiler->compileError = (CompileError)getErrorLexer(compiler->lexer);
            break;
        }

        if (isEmptyQueue(compiler->lexemesInfix)) continue;

        printf("\nNew expr infix:\n");
        printLexemesQueue(compiler->lexemesInfix);

        bool scopeProcSuccess = processScopes(compiler, false);
        if (!scopeProcSuccess) break;

        if (isEmptyQueue(compiler->lexemesInfix)) continue;

        // регистрация функции
        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_KEYWORD &&
            strcmp(bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))), KEYWORD_FUNCTION) == 0) {

            if (!isEmptyStack(compiler->scopesOpenings)) {
                // ошибка - функция может быть объявлена только на верхнем уровне вложенности
                // валидация
            }

            if (sizeQueue(compiler->lexemesInfix) == 1) {
                compiler->compileError = (CompileError)createSyntaxError(
                    posLexeme(topQueue(Lexeme, compiler->lexemesInfix)),
                    contentLexeme(topQueue(Lexeme, compiler->lexemesInfix)),
                    SYNTAX_ERROR_UNEXPECTED_LEXEME
                );

                break;
            }

            bool regFuncSuccess = registrateFunction(compiler);
            if (!regFuncSuccess) break;

            // если это начало функции - требуется произвести обработку скобок повторно
            bool scopeProcSuccess = processScopes(compiler, true);
            if (!scopeProcSuccess) break;
        }

        // регистрация имени
        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_NAME &&
            hasMap(compiler->typeTable, bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix)))) &&
            !hasStrSet(compiler->functions, bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))))) {
            
            // если дальше ничего нет - пропускаем
            if (sizeQueue(compiler->lexemesInfix) == 1) {
                deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));
                continue;
            }
            
            bool nameRegSuccess = registrateName(compiler);
            if (!nameRegSuccess) break;
        }

        bool needReturn = false;

        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_KEYWORD &&
            strcmp(bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))), KEYWORD_RETURN) == 0) {

            if (compiler->currentFunction == NULL) {
                // ошибка: return не может быть использован вне тела функции
                // валидация
            }

            needReturn = true;
            deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));
        }

        bool postfixTranslationSuccess = translateToPostfix(
            compiler->analyzer,
            compiler->lexemesInfix, 
            compiler->lexemesPostfix, 
            compiler->functions,
            TILL_END
        );
        
        if (!postfixTranslationSuccess) {
            compiler->compileError = (CompileError)getSyntaxError(compiler->analyzer);
            break;
        }

        printf("\nNew expr postfix:\n");   
        printLexemesQueue(compiler->lexemesPostfix);
        printf("\n");

        resetBinCodeGen(compiler->codeGen);

        bool codeGenSuccess;
        REGISTER exprRes = postfixToBinCode(
            compiler->codeGen, 
            compiler->lexemesPostfix, 
            compiler->nameTable, 
            compiler->typeTable, 
            compiler->analyzer,
            &codeGenSuccess
        );

        if (!codeGenSuccess) {
            compiler->compileError = genErrorBinCodeGen(compiler->codeGen);
            break;
        }

        if (needReturn) {
            generateReturnBinCodeGen(compiler->codeGen, compiler->typeTable, compiler->currentFunction, exprRes);
        }
    } while (!feof(in));

    backpatchLinks(compiler->codeGen);

    if (!isEmptyStack(compiler->scopesOpenings) && compiler->compileError == NULL) {
        compiler->compileError = (CompileError)createSyntaxError(
            posLexeme(lexemeScopeOpen(topStack(ScopeOpen, compiler->scopesOpenings))),
            contentLexeme(lexemeScopeOpen(topStack(ScopeOpen, compiler->scopesOpenings))),
            SYNTAX_ERROR_UNCLOSED_BLOCK
        );
    }

    return compiler->compileError == NULL;
}

void setUnexpectedTopLexeme (Compiler compiler) {
    compiler->compileError = (CompileError)createSyntaxError(
        posLexeme(topQueue(Lexeme, compiler->lexemesInfix)), 
        contentLexeme(topQueue(Lexeme, compiler->lexemesInfix)), 
        SYNTAX_ERROR_UNEXPECTED_LEXEME
    );
}

bool registrateName (Compiler compiler) {
    Lexeme topLexeme = popQueue(Lexeme, compiler->lexemesInfix);
    char *type = bufferString(contentLexeme(topLexeme));

    if (isEmptyQueue(compiler->lexemesInfix) || typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_NAME) {
        deleteLexeme(topLexeme);

        setUnexpectedTopLexeme(compiler);

        return false;
    }

    // если следующая лексема является именем типа - ошибка
    if (hasMap(compiler->typeTable, bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))))) {
        deleteLexeme(topLexeme);

        Lexeme unexpectedLexeme = topQueue(Lexeme, compiler->lexemesInfix);

        compiler->compileError = (CompileError)createTypeError(
            posLexeme(unexpectedLexeme), 
            contentLexeme(unexpectedLexeme), 
            TYPE_ERROR_MULTIPLE_TYPES_IN_DECLARATION
        );

        return false;
    }

    NameErrorType addRes = addNameToPriorityTable(
        compiler->nameTable, 
        bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))), 
        type, 
        compiler->typeTable
    );

    if (addRes != NAME_ADDING_SUCCESS) {
        deleteLexeme(topLexeme);

        Lexeme unexpectedLexeme = topQueue(Lexeme, compiler->lexemesInfix);

        compiler->compileError = (CompileError)createNameError(
            posLexeme(unexpectedLexeme),
            contentLexeme(unexpectedLexeme),
            addRes
        );

        return false;
    }

    deleteLexeme(topLexeme);
    return true;
}

bool registrateFunction (Compiler compiler) {
    // создаем локальный раздел таблицы имен 
    addBaseSectionToPriorityTable(compiler->nameTable);

    // удаляем лексему с ключевым словом
    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    if (isEmptyQueue(compiler->lexemesInfix)) {
        // Ошибка: требуется тип возвращаемого значения
        // валидация
    }
    
    List funcTypeComponents = createList(); 
    
    // добавить тип возвращаемого значения в список компонентов типа
    insertToList(funcTypeComponents, endList(funcTypeComponents), createStringCopy(
        contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))
    ));

    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    if (isEmptyQueue(compiler->lexemesInfix)) {
        // Ошибка: требуется имя функции
        // валидация

        for (ListIter it = beginList(funcTypeComponents); iterNotEquals(it, endList(funcTypeComponents)); it = iterNext(it)) {
            deleteString(iterFetch(string, it));
        }

        deleteList(funcTypeComponents);
        return false;
    }

    // считать имя функции
    string funcName = createStringCopy(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix)));

    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    // является ли функция точкой входа
    bool isEntry = strcmp(bufferString(funcName), ENTRY_NAME) == 0;
    
    if (isEmptyQueue(compiler->lexemesInfix)) {
        // Ошибка: отсутствует список аргументов
        // валидация

        for (ListIter it = beginList(funcTypeComponents); iterNotEquals(it, endList(funcTypeComponents)); it = iterNext(it)) {
            deleteString(iterFetch(string, it));
        }

        deleteList(funcTypeComponents);
        deleteString(funcName);
        return false;
    }

    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_OPEN_BRACKET) {
        compiler->compileError = (CompileError)createSyntaxError(
            posLexeme(topQueue(Lexeme, compiler->lexemesInfix)),
            contentLexeme(topQueue(Lexeme, compiler->lexemesInfix)),
            SYNTAX_ERROR_UNEXPECTED_LEXEME
        );

        for (ListIter it = beginList(funcTypeComponents); iterNotEquals(it, endList(funcTypeComponents)); it = iterNext(it)) {
            deleteString(iterFetch(string, it));
        }

        deleteList(funcTypeComponents);
        deleteString(funcName);
        return false;
    }

    // считать скобку, открывающую список аргументов
    Lexeme openBracket = popQueue(Lexeme, compiler->lexemesInfix);

    // если функция является точкой входа, она не должна принимать аргументов
    if (isEntry && (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_CLOSE_BRACKET || 
        isEmptyQueue(compiler->lexemesInfix))) {
        // если принимает - ошибка
        // валидация
    }

    while (true) {
        // пустой очередь быть не может: должна быть закрывающая скобка
        if (isEmptyQueue(compiler->lexemesInfix)) {
            // если пустая - ошибка
            compiler->compileError = (CompileError)createSyntaxError(
                posLexeme(openBracket),
                contentLexeme(openBracket),
                SYNTAX_ERROR_UNCLOSED_BRACKET
            );

            deleteLexeme(openBracket);

            for (ListIter it = beginList(funcTypeComponents); iterNotEquals(it, endList(funcTypeComponents)); it = iterNext(it)) {
                deleteString(iterFetch(string, it));
            }

            deleteList(funcTypeComponents);
            deleteString(funcName);
            return false;
        }

        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_EXPR_CLOSE_BRACKET) {
            break;
        }

        // чтение типа аргумента
        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_NAME) {
            // ошибка - неожиданная лексема
            // валидация
        }

        if (!hasMap(compiler->typeTable, bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))))) {
            // ошибка - неизвестный тип
            // валидация
        }

        Lexeme argType = popQueue(Lexeme, compiler->lexemesInfix);

        insertToList(funcTypeComponents, iterPrev(endList(funcTypeComponents)), 
            createStringCopy(contentLexeme(argType))
        );

        // чтение имени аргумента
        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_NAME) {
            // ошибка - неожиданная лексема
            // валидация
        }

        Lexeme argName = popQueue(Lexeme, compiler->lexemesInfix);

        // регистрируем имя в таблице имен
        NameErrorType addRes = addNameToPriorityTable(
            compiler->nameTable, 
            bufferString(contentLexeme(argName)), 
            bufferString(contentLexeme(argType)),
            compiler->typeTable
        );

        if (addRes != NAME_ADDING_SUCCESS) {
            compiler->compileError = (CompileError)createNameError(
                posLexeme(argName),
                contentLexeme(argName),
                addRes
            );

            deleteLexeme(argType);
            deleteLexeme(argName);
            deleteLexeme(openBracket);

            for (ListIter it = beginList(funcTypeComponents); iterNotEquals(it, endList(funcTypeComponents)); it = iterNext(it)) {
                deleteString(iterFetch(string, it));
            }

            deleteList(funcTypeComponents);
            deleteString(funcName);

            return false;
        }

        deleteLexeme(argType);
        deleteLexeme(argName);

        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_ARG_SEP) {
            // выбросить разделитель
            deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));
        } else if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_CLOSE_BRACKET) {
            // если не было разделителя - дальше должна быть закрывающая скобка
            // если ее нет - ошибка
            // валидация
        } 
    }

    // удалить закрывающую скобку
    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));
    
    // удалить открывающую скобку
    deleteLexeme(openBracket);

    // создаем тип
    Type funcType = createType(2, funcTypeComponents);

    // и регистрируем его
    setMap(compiler->typeTable, bufferString(funcName), funcType);

    // запоминаем адрес функции
    functionHereBinCodeGen(compiler->codeGen, bufferString(funcName));

    // запоминаем, что имя funcName является именем функции, и устанавливаем его в качестве имене текущей функции
    addStrSet(compiler->functions, bufferString(funcName));
    compiler->currentFunction = funcName;

    printType(getMap(Type, compiler->typeTable, bufferString(funcName)));
    printf("\n");

    return true;
}

bool isLexemeOpensScope (LexType type) {
    return type == LEX_BLOCK_OPEN_BRACKET || type == LEX_KEYWORD;
}

bool processScopes (Compiler compiler, bool funcStarts) {
    // закрытие старых блоков
    while (!isEmptyQueue(compiler->lexemesInfix) && 
        typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_BLOCK_CLOSE_BRACKET) {

        if (isEmptyStack(compiler->scopesOpenings)) {
            compiler->compileError = (CompileError)createSyntaxError(
                posLexeme(lexemeScopeOpen(topQueue(ScopeOpen, compiler->lexemesInfix))),
                contentLexeme(lexemeScopeOpen(topQueue(ScopeOpen, compiler->lexemesInfix))),
                SYNTAX_ERROR_UNOPENED_BLOCK
            );

            return false;
        }

        switch (typeScopeOpen(topStack(ScopeOpen, compiler->scopesOpenings))) {
            case SCOPE_IF:
                closeBranch(compiler->codeGen);
                break;

            case SCOPE_WHILE:
                closeLoop(compiler->codeGen);
                break;
            
            case SCOPE_FUNCTION:
                // блок функции завершен, текущая функция - никакая
                deleteString(compiler->currentFunction);
                compiler->currentFunction = NULL;
                break;
        
            default:
                break;
        }

        deleteScopeOpen(popStack(ScopeOpen, compiler->scopesOpenings));
        deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));
        popTopSectionFromPriorityTable(compiler->nameTable);
    }

    // открытие новых блоков
    while (!isEmptyQueue(compiler->lexemesInfix) && isLexemeOpensScope(typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)))) {
        if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) == LEX_KEYWORD) {
            // если начало функции - прекращение обработки блоков
            if (strcmp(bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))), KEYWORD_FUNCTION) == 0) {
                break;
            } 

            // если return - прекращение обработки блоков
            if (strcmp(bufferString(contentLexeme(topQueue(Lexeme, compiler->lexemesInfix))), KEYWORD_RETURN) == 0) {
                break;
            } 
        }

        Lexeme topLex = popQueue(Lexeme, compiler->lexemesInfix);
        LexType lexType = typeLexeme(topLex);
        ScopeType scopeType;

        if (lexType == LEX_BLOCK_OPEN_BRACKET) {
            if (funcStarts) {
                scopeType = SCOPE_FUNCTION;
                funcStarts = false;
            } else scopeType = SCOPE_BLOCK;
        } else if (lexType == LEX_KEYWORD) {
            string content = contentLexeme(topLex);

            if (strcmp(bufferString(content), KEYWORD_IF) == 0) {
                scopeType = SCOPE_IF;
                bool conditionCompileSuccess = compileBranchCondition(compiler);
                
                if (!conditionCompileSuccess) {
                    deleteLexeme(topLex);
                    return false;
                } 
            }

            if (strcmp(bufferString(content), KEYWORD_WHILE) == 0) {
                scopeType = SCOPE_WHILE;
                bool conditionCompileSuccess = compileLoopCondition(compiler);

                if (!conditionCompileSuccess) {
                    deleteLexeme(topLex);
                    return false;
                } 
            }
        }

        pushStack(compiler->scopesOpenings, createScopeOpen(topLex, scopeType));

        // блок для функции создается при регистрации
        if (scopeType != SCOPE_FUNCTION) addLinkedSectionToPriorityTable(compiler->nameTable);
    }

    return true;
}

bool compileBranchCondition (Compiler compiler) {
    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_OPEN_BRACKET) {
        setUnexpectedTopLexeme(compiler);
        return false;
    } 
    
    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    bool postfixTranslationSuccess = translateToPostfix(
        compiler->analyzer,
        compiler->lexemesInfix, 
        compiler->lexemesPostfix, 
        compiler->functions,
        TILL_RECURSIVE_CLOSE
    );
    
    if (!postfixTranslationSuccess) {
        compiler->compileError = (CompileError)getSyntaxError(compiler->analyzer);
        return false;
    }

    printf("\nNew condition postfix:\n");   
    printLexemesQueue(compiler->lexemesPostfix);
    printf("\n");

    resetBinCodeGen(compiler->codeGen);

    bool codeGenSuccess;
    REGISTER conditionValueReg = postfixToBinCode(
        compiler->codeGen, 
        compiler->lexemesPostfix, 
        compiler->nameTable, 
        compiler->typeTable,
        compiler->analyzer,
        &codeGenSuccess
    );

    if (!codeGenSuccess) {
        compiler->compileError = genErrorBinCodeGen(compiler->codeGen);
        return false;
    }

    addBranchBinCodeGen(compiler->codeGen, conditionValueReg);

    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_CLOSE_BRACKET) {
        setUnexpectedTopLexeme(compiler);
        return false;
    }

    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_BLOCK_OPEN_BRACKET) {
        setUnexpectedTopLexeme(compiler);
        return false;
    }

    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    return true;
}

bool compileLoopCondition (Compiler compiler) {
    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_OPEN_BRACKET) {
        setUnexpectedTopLexeme(compiler);
        return false;
    } 
    
    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    addLoopLinkBinCodeGen(compiler->codeGen);

    bool postfixTranslationSuccess = translateToPostfix(
        compiler->analyzer,
        compiler->lexemesInfix, 
        compiler->lexemesPostfix, 
        compiler->functions,
        TILL_RECURSIVE_CLOSE
    );
    
    if (!postfixTranslationSuccess) {
        compiler->compileError = (CompileError)getSyntaxError(compiler->analyzer);
        return false;
    }

    printf("\nNew condition postfix:\n");   
    printLexemesQueue(compiler->lexemesPostfix);
    printf("\n");

    resetBinCodeGen(compiler->codeGen);

    bool codeGenSuccess;
    REGISTER conditionValueReg = postfixToBinCode(
        compiler->codeGen, 
        compiler->lexemesPostfix, 
        compiler->nameTable, 
        compiler->typeTable, 
        compiler->analyzer,
        &codeGenSuccess
    );

    if (!codeGenSuccess) {
        compiler->compileError = genErrorBinCodeGen(compiler->codeGen);
        return false;
    }

    addLoopConditionBinCodeGen(compiler->codeGen, conditionValueReg);

    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_EXPR_CLOSE_BRACKET) {
        setUnexpectedTopLexeme(compiler);
        return false;
    }

    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    if (typeLexeme(topQueue(Lexeme, compiler->lexemesInfix)) != LEX_BLOCK_OPEN_BRACKET) {
        setUnexpectedTopLexeme(compiler);
        return false;
    }

    deleteLexeme(popQueue(Lexeme, compiler->lexemesInfix));

    return true;
}

void typeDestructor (void *object) {
    deleteType((Type)object);
}

void printLexemesQueue (Queue queue) {
    for (int i = 0; i < sizeQueue(queue); i++) {
        printLexeme(topQueue(Lexeme, queue)); printf("\n");
        pushQueue(queue, popQueue(Lexeme, queue));
    }
}

ScopeOpen createScopeOpen (Lexeme lexeme, ScopeType type) {
    ScopeOpen scope = (ScopeOpen)malloc(sizeof(_ScopeOpen));
    scope->lexeme = lexeme;
    scope->type = type;
}

void deleteScopeOpen (ScopeOpen scope) {
    deleteLexeme(scope->lexeme);
    free(scope);
}

ScopeType typeScopeOpen (ScopeOpen scope) {
    return scope->type;
}

Lexeme lexemeScopeOpen (ScopeOpen scope) {
    return scope->lexeme;
}
