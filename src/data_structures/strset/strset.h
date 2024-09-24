#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct __StrSetNode {
    char *key;
    struct __StrSetNode *l;
    struct __StrSetNode *r;
} __StrSetNode;

typedef struct {
    __StrSetNode *root;
} _StrSet, *StrSet;

StrSet createStrSet ();
void deleteStrSet (StrSet set);
bool addStrSet (StrSet set, char *inserting);
bool hasStrSet (StrSet set, char *finding);
void printStrSet (StrSet set);