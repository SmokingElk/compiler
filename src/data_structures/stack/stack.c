#include "stack.h"

#define STACK_ADD_SIZE 3

void resizeStack (Stack stack, int dCapacity);

Stack createStack (int capacity) {
    Stack stack = (Stack)malloc(sizeof(_Stack));
    stack->topIndex = -1;
    stack->capacity = capacity;

    if (capacity == 0) stack->elems = NULL;
    stack->elems = (STACK_T*)malloc(sizeof(STACK_T) * capacity);

    return stack;
}

void deleteStack (Stack stack) {
    if (stack->elems != NULL) free(stack->elems); 
    free(stack);
}

bool isEmptyStack (Stack stack) {
    return stack->topIndex == -1;
}

int sizeStack (Stack stack) {
    return stack->topIndex + 1;
}

STACK_T __topStack (Stack stack) {
    if (isEmptyStack(stack)) return NULL;
    return stack->elems[stack->topIndex];
}

STACK_T __popStack (Stack stack) {
    if (isEmptyStack(stack)) return NULL;
    return stack->elems[stack->topIndex--];
}

void __pushStack (Stack stack, STACK_T elem) {
    if (stack->topIndex + 1 >= stack->capacity) resizeStack(stack, STACK_ADD_SIZE);
    stack->elems[++stack->topIndex] = elem;
}

void resizeStack (Stack stack, int dCapacity) {
    int newCapacity = stack->capacity + dCapacity;
    if (newCapacity < 0) newCapacity = 0;

    STACK_T *newElems;
    if (newCapacity == 0) newElems = NULL;
    else newElems = (STACK_T*)malloc(sizeof(STACK_T) * newCapacity);

    for (int i = 0; i < sizeStack(stack) && i < newCapacity; i++) {
        newElems[i] = stack->elems[i];
    }

    if (stack->elems != NULL) free(stack->elems);
    stack->elems = newElems;
    stack->capacity = newCapacity;
}