#include "strset.h"

StrSet createStrSet () {
    StrSet res = (StrSet)malloc(sizeof(_StrSet));
    res->root = NULL;
    return res;
}

void __deleteStrSetRec (__StrSetNode *root) {
    if (root == NULL) return;

    __deleteStrSetRec(root->l);
    __deleteStrSetRec(root->r);

    free(root->key);
    free(root);
}

void deleteStrSet (StrSet set) {
    __deleteStrSetRec(set->root);
    free(set);
}

bool addStrSet (StrSet set, char *inserting) {
    __StrSetNode **node = &set->root;

    while (*node != NULL) {
        int cmp = strcmp((*node)->key, inserting);

        if (cmp == 0) return false;
        if (cmp == -1) node = &(*node)->l;
        else node = &(*node)->r;
    }
    
    *node = (__StrSetNode*)malloc(sizeof(__StrSetNode));
    (*node)->l = NULL;
    (*node)->r = NULL;
    (*node)->key = (char*)malloc(sizeof(char) * (strlen(inserting) + 1));
    strcpy((*node)->key, inserting);

    return true;
}

bool hasStrSet (StrSet set, char *finding) {
    __StrSetNode *node = set->root;

    while (node != NULL) {
        int cmp = strcmp(node->key, finding);
        
        if (cmp == 0) return true;
        if (cmp < 0) node = node->l;
        else node = node->r;
    }

    return false;
}

void __printSetRec (__StrSetNode *node) {
    if (node == NULL) return;

    printf("%s ", node->key);
    __printSetRec(node->l);
    __printSetRec(node->r);
}

void printStrSet (StrSet set) {
    __printSetRec(set->root);
}