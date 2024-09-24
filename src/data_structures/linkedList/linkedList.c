#include "linkedList.h"

ListNode createNode (ListElem value) {
    ListNode res = (ListNode)malloc(sizeof(_ListNode));
    res->value = value;
    res->next = NULL;
    res->prev = NULL;
    return res;
}

void deleteNode (ListNode node) {
    free(node);
}

ListIter createIter (ListNode node) {
    ListIter res = {node};
    return res;
}

ListIter iterNext (ListIter iter) {
    iter.node = iter.node->next;
    return iter;
}

ListIter iterPrev (ListIter iter) {
    iter.node = iter.node->prev;
    return iter;
}

bool iterEquals (ListIter iter1, ListIter iter2) {
    return iter1.node == iter2.node;
}

bool iterNotEquals (ListIter iter1, ListIter iter2) {
    return iter1.node != iter2.node;
}

ListElem __iterFetch (ListIter iter) {
    return iter.node->value;
}

void __iterStore (ListIter iter, ListElem value) {
    iter.node->value = value;
}

List createList () {
    List res = (List)malloc(sizeof(_List));
    ListNode term = createNode(NULL);
    term->next = term;
    term->prev = term;
    res->begin = createIter(term);
    res->end = createIter(term);
    return res;
}

void deleteList (List list) {
    ListIter it = beginList(list);

    while (iterNotEquals(it, endList(list))) {
        ListNode ptn = it.node;
        it = iterNext(it);
        deleteNode(ptn);
    }
    
    deleteNode(it.node);
    free(list);
}

ListIter beginList (List list) {
    return list->begin;
}

ListIter endList (List list) {
    return list->end;
}

bool isEmptyList (List list) {
    return iterEquals(beginList(list), endList(list));
}

bool inList (List list, ListIter finding) {
    for (ListIter it = beginList(list); iterNotEquals(it, endList(list)); it = iterNext(it)) {
        if (iterEquals(it, finding)) return true;
    }

    return iterEquals(finding, endList(list));
}

ListIter __findList (List list, ListElem value) {
    for (ListIter it = beginList(list); iterNotEquals(it, endList(list)); it = iterNext(it)) {
        if (iterFetch(ListElem, it) == value) return it;
    }

    return endList(list);
}

bool __insertToList (List list, ListIter after, ListElem value) {
    ListIter next = iterNext(after);

    ListNode newNode = createNode(value);
    after.node->next = newNode;
    newNode->next = next.node;
    next.node->prev = newNode;
    newNode->prev = after.node;

    if (iterEquals(after, endList(list))) list->begin = createIter(newNode);

    return true;
}

bool deleteFromList (List list, ListIter deleting) {
    if (!inList(list, deleting)) return false;
    if (iterEquals(deleting, endList(list))) return false;

    ListIter delPrev = iterPrev(deleting);
    ListIter delNext = iterNext(deleting);

    delPrev.node->next = delNext.node;
    delNext.node->prev = delPrev.node;

    if (iterEquals(deleting, beginList(list))) list->begin = delNext;

    free(deleting.node);
    return true;
}

int lengthList (List list) {
    int length = 0;

    for (ListIter it = beginList(list); iterNotEquals(it, endList(list)); it = iterNext(it)) {
        length++;
    }

    return length;
}
