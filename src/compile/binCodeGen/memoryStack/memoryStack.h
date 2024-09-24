#pragma once

#include "../../instruction/instruction.h"
#include <stdlib.h>

typedef struct {
    word top;
} _MemoryStack, *MemoryStack;

MemoryStack createMemoryStack (word top);
void deleteMemoryStack (MemoryStack stack);
instruction pushMemoryStack (MemoryStack stack, MEM_MODE mode, REGISTER reg);
instruction popMemoryStack (MemoryStack stack, MEM_MODE mode, REGISTER reg);
word getTopMemoryStack (MemoryStack stack);