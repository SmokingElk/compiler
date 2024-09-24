#include "../binCodeGen.h"

Operand mapperAnd (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand1 = iterFetch(Operand, beginList(operands));
    Operand operand2 = iterFetch(Operand, iterNext(beginList(operands)));

    REGISTER reg1 = valueRegOperand(operand1);
    REGISTER reg2 = valueRegOperand(operand2);

    instruction instr = i_ALUR(ALU_AND, reg1, reg2);
    writeInstruction(gen, instr);

    freeOperandReg(gen, operand2);

    return createOperand(valueLocOperand(operand1), 0, typeOperand(operand1));
}