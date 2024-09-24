#include "../binCodeGen.h"

Operand mapperNot (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand = iterFetch(Operand, beginList(operands));
    REGISTER reg = valueRegOperand(operand);

    // сравниваем с false
    instruction instr = i_CMPC(reg, 0);
    writeInstruction(gen, instr);

    // если равно - прыжок на запись true
    instr = i_BCON(CMP_EQ, shiftInstrAddr(gen->instrWritten, 3));
    writeInstruction(gen, instr);

    // если нет - пишем false
    instr = i_WRITE(reg, 0);
    writeInstruction(gen, instr);

    // прыжок за запись true
    instr = i_JUMP(shiftInstrAddr(gen->instrWritten, 2));
    writeInstruction(gen, instr);

    // запись true
    instr = i_WRITE(reg, 1);
    writeInstruction(gen, instr);

    return createOperand(valueLocOperand(operand), 0, typeOperand(operand));
}