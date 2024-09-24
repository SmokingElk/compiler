#include "utilTypesBinCodeGen.h"

LOCATION createLoc (REGISTER reg) {
    LOCATION loc = (LOCATION)malloc(sizeof(REGISTER));
    *loc = reg;
    return loc;
}

REGISTER getReg (LOCATION loc) {
    return *loc;
}

ADDR createAddr (word value) {
    ADDR res = (ADDR)malloc(sizeof(word));
    *res = value;
    return res;
}

word readAddr (ADDR addr) {
    return *addr;
}

void deleteAddr (ADDR addr) {
    free(addr);
}

void addrDestructor (void *object) {
    free((ADDR)object);
}

Operand createOperand (LOCATION valueLoc, word srcAddr, Type type) {
    Operand operand = (Operand)malloc(sizeof(_Operand));
    operand->valueLoc = valueLoc;
    operand->srcAddr = srcAddr;
    operand->type = type;
    return operand;
}

void deleteOperand (Operand operand) {
    free(operand);
}

LOCATION valueLocOperand (Operand operand) {
    return operand->valueLoc;
}

REGISTER valueRegOperand (Operand operand) {
    return getReg(operand->valueLoc);
}

word srcAddrOperand (Operand operand) {
    return operand->srcAddr;
}

Type typeOperand (Operand operand) {
    return operand->type;
}