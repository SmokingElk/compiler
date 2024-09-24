#include "../binCodeGen.h"

Operand mapperPrint (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand = iterFetch(Operand, beginList(operands));
    
    REGISTER reg = valueRegOperand(operand);

    instruction instr = i_MOVE(REG_AO, reg);
    writeInstruction(gen, instr);

    freeOperandReg(gen, operand);
    return NULL;
}