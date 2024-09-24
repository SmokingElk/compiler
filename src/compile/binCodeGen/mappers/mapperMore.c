#include "../binCodeGen.h"

Operand mapperMore (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand1 = iterFetch(Operand, beginList(operands));
    Operand operand2 = iterFetch(Operand, iterNext(beginList(operands)));

    REGISTER reg1 = valueRegOperand(operand1);
    REGISTER reg2 = valueRegOperand(operand2);
    instruction instr;

    // a > b <=> b < a
    instr = i_CMPR(reg2, reg1);
    writeInstruction(gen, instr);

    instr = i_MOVE(reg2, REG_RS);
    writeInstruction(gen, instr);

    // 1 - битовая маска 0b00..01. Обнуляет все биты, кроме 1-о
    instr = i_ALUC(ALU_AND, reg2, 1);
    writeInstruction(gen, instr);

    freeOperandReg(gen, operand1);

    return createOperand(valueLocOperand(operand2), 0, getMap(Type, typeTable, BASE_TYPE_BOOL));
}