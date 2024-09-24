#include "memoryStack.h"

MemoryStack createMemoryStack (word top) {
    MemoryStack res = (MemoryStack)malloc(sizeof(_MemoryStack));
    res->top = top;
}

void deleteMemoryStack (MemoryStack stack) {
    free(stack);
}

instruction pushMemoryStack (MemoryStack stack, MEM_MODE mode, REGISTER reg) {
    // фу, костыль
    int size = mode == MEM_WORD_16 ? 2 : 1;
    stack->top -= size;

    return i_SAVER(stack->top + 1, mode, reg);
}

instruction popMemoryStack (MemoryStack stack, MEM_MODE mode, REGISTER reg) {
    int size = mode == MEM_WORD_16 ? 2 : 1;
    stack->top += size;

    return i_LOAD(reg, stack->top + 1 - size, mode); 
}

word getTopMemoryStack (MemoryStack stack) {
    return stack->top;
}