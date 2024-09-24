#include "../binCodeGen.h"

Operand mapperEqual (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand1 = iterFetch(Operand, beginList(operands));
    Operand operand2 = iterFetch(Operand, iterNext(beginList(operands)));

    REGISTER reg1 = valueRegOperand(operand1);
    REGISTER reg2 = valueRegOperand(operand2);
    instruction instr;

    instr = i_CMPR(reg1, reg2);
    writeInstruction(gen, instr);

    instr = i_MOVE(reg1, REG_RS);
    writeInstruction(gen, instr);

    // 2 - битовая маска 0b00..010. Обнуляет все биты, кроме 2-о
    instr = i_ALUC(ALU_AND, reg1, 2);
    writeInstruction(gen, instr);

    // смещение на результата на 1-й бит
    instr = i_ALUC(ALU_SRL, reg1, 0);
    writeInstruction(gen, instr);

    freeOperandReg(gen, operand2);

    return createOperand(valueLocOperand(operand1), 0, getMap(Type, typeTable, BASE_TYPE_BOOL));
}