#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef void* ListElem;

typedef struct __ListNode {
    ListElem value;
    struct __ListNode *next;
    struct __ListNode *prev;
} _ListNode;

typedef _ListNode* ListNode;

typedef struct {
    ListNode node;
} ListIter;

typedef struct {
    ListIter begin;
    ListIter end;
} _List;

typedef _List* List;

ListNode createNode (ListElem value);
void deleteNode (ListNode node);

ListIter createIter (ListNode node);
ListIter iterNext (ListIter iter);
ListIter iterPrev (ListIter iter);
bool iterEquals (ListIter iter1, ListIter iter2);
bool iterNotEquals (ListIter iter1, ListIter iter2);
ListElem __iterFetch (ListIter iter);
void __iterStore (ListIter iter, ListElem value);

List createList ();
void deleteList (List list);
ListIter beginList (List list);
ListIter endList (List list);
bool isEmptyList (List list);
int lengthList (List list);
ListIter __findList (List list, ListElem value);
bool inList (List list, ListIter finding);
bool __insertToList (List list, ListIter after, ListElem value);
bool deleteFromList (List list, ListIter deleting);

#define findList(list, value) __findList((list), (ListElem)(value))
#define insertToList(list, after, value) __insertToList ((list), (after), (ListElem)(value))
#define iterFetch(ValueType, iter) (ValueType)__iterFetch((iter))
#define iterStore(iter, value) __iterStore((iter), (ListElem)(value))