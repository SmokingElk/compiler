#include "queue.h"

#define QUEUE_ADD_SIZE 10

void resizeQueue (Queue queue, int dCapacity);

Queue createQueue (int capacity) {
    Queue res = (Queue)malloc(sizeof(_Queue));

    res->topIndex = 0;
    res->freeIndex = 0;
    res->size = 0;
    res->capacity = capacity;
    res->elems = (QUEUE_T*)malloc(sizeof(QUEUE_T) * capacity);

    return res;
}

void deleteQueue (Queue queue) {
    free(queue->elems);
    free(queue);
}

bool isEmptyQueue (Queue queue) {
    return queue->size == 0;
}

int sizeQueue (Queue queue) {
    return queue->size;
}

QUEUE_T __topQueue (Queue queue) {
    if (isEmptyQueue(queue)) return NULL;
    return queue->elems[queue->topIndex];
}

QUEUE_T __popQueue (Queue queue) {
    if (isEmptyQueue(queue)) return NULL;
    
    QUEUE_T top = __topQueue(queue);
    queue->topIndex = (queue->topIndex + 1) % queue->capacity;
    queue->size--;

    return top; 
}   

void __pushQueue (Queue queue, QUEUE_T elem) {
    if (sizeQueue(queue) >= queue->capacity) resizeQueue(queue, QUEUE_ADD_SIZE);

    queue->elems[queue->freeIndex] = elem;
    queue->freeIndex = (queue->freeIndex + 1) % queue->capacity;
    queue->size++;
}

void resizeQueue (Queue queue, int dCapacity) {
    int newCapacity = queue->capacity + dCapacity;
    if (newCapacity < 0) newCapacity = 0;

    QUEUE_T *newElems = (QUEUE_T*)malloc(sizeof(QUEUE_T) * newCapacity);
    
    for (int i = 0; i < sizeQueue(queue) && i < newCapacity; i++) {
        newElems[i] = queue->elems[(queue->topIndex + i) % queue->capacity];
    }

    free(queue->elems);
    queue->elems = newElems;

    queue->freeIndex = sizeQueue(queue);
    queue->topIndex = 0;
    queue->capacity = newCapacity;
}