#pragma once
#include "stdlib.h"
#include "../../instruction/instruction.h"
#include "../../type/type.h"
#include "../../../data_structures/map/map.h"
#include "../../../data_structures/linkedList/linkedList.h"

typedef REGISTER* LOCATION;
typedef word* ADDR;

typedef struct {
    LOCATION valueLoc;
    word srcAddr;
    Type type;
} _Operand, *Operand;

LOCATION createLoc (REGISTER reg);
REGISTER getReg (LOCATION loc);

ADDR createAddr (word value);
word readAddr (ADDR addr);
void deleteAddr (ADDR addr);
void addrDestructor (void *object);

Operand createOperand (LOCATION valueLoc, word srcAddr, Type type);
void deleteOperand (Operand operand);
LOCATION valueLocOperand (Operand operand);
REGISTER valueRegOperand (Operand operand);
word srcAddrOperand (Operand operand);
Type typeOperand (Operand operand);