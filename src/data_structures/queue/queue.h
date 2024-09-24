#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef void* QUEUE_T;

typedef struct {
    QUEUE_T *elems;
    int topIndex;
    int freeIndex; 
    int size;
    int capacity;
} _Queue, *Queue;

Queue createQueue (int capacity);
void deleteQueue (Queue queue);
bool isEmptyQueue (Queue queue);
int sizeQueue (Queue queue);
#define pushQueue(queue, value) __pushQueue((queue), (QUEUE_T)(value))
#define topQueue(type, queue) (type)__topQueue((queue))
#define popQueue(type, queue) (type)__popQueue((queue))

QUEUE_T __topQueue (Queue queue);
QUEUE_T __popQueue (Queue queue);
void __pushQueue (Queue queue, QUEUE_T elem);