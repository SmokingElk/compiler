#include "../binCodeGen.h"

Operand mapperMoreEqual (BinCodeGen gen, List operands, Map typeTable) {
    Operand operand1 = iterFetch(Operand, beginList(operands));
    Operand operand2 = iterFetch(Operand, iterNext(beginList(operands)));

    REGISTER reg1 = valueRegOperand(operand1);
    REGISTER reg2 = valueRegOperand(operand2);
    instruction instr;

    // сравнить операнды
    instr = i_CMPR(reg1, reg2);
    writeInstruction(gen, instr);

    // если не равны - прыжок на запись true
    instr = i_BCON(CMP_ME, shiftInstrAddr(gen->instrWritten, 3));
    writeInstruction(gen, instr);

    // если нет - пишем false
    instr = i_WRITE(reg1, 0);
    writeInstruction(gen, instr);

    // прыжок за запись true
    instr = i_JUMP(shiftInstrAddr(gen->instrWritten, 2));
    writeInstruction(gen, instr);

    // запись true
    instr = i_WRITE(reg1, 1);
    writeInstruction(gen, instr);

    freeOperandReg(gen, operand2);

    return createOperand(valueLocOperand(operand1), 0, getMap(Type, typeTable, BASE_TYPE_BOOL));
}