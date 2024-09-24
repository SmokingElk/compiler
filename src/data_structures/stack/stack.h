#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

typedef void* STACK_T;

typedef struct {
    STACK_T *elems;
    int topIndex;
    int capacity;
} _Stack, *Stack;

Stack createStack (int capacity);
void deleteStack (Stack stack);
bool isEmptyStack (Stack stack);
int sizeStack (Stack stack);
#define pushStack(stack, value) __pushStack((stack), (STACK_T)(value))
#define topStack(type, stack) (type)__topStack((stack))
#define popStack(type, stack) (type)__popStack((stack))

STACK_T __topStack (Stack stack);
STACK_T __popStack (Stack stack);
void __pushStack (Stack stack, STACK_T elem);