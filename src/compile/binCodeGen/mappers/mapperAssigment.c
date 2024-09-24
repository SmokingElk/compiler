#include "../binCodeGen.h"

Operand mapperAssigment (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand1 = iterFetch(Operand, beginList(operands));
    Operand operand2 = iterFetch(Operand, iterNext(beginList(operands)));

    REGISTER reg2 = valueRegOperand(operand2);

    instruction instr = i_SAVER(srcAddrOperand(operand1), memModeType(typeOperand(operand1)), reg2);
    writeInstruction(gen, instr);

    freeOperandReg(gen, operand2);

    return createOperand(valueLocOperand(operand1), srcAddrOperand(operand1), typeOperand(operand1));
}