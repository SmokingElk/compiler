#pragma once
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct {
    char *buffer;
} _string, *string;

string createString (char* str);
string createStringCopy (string str);
void deleteString (string str);
int lenString (string str);
bool equalString (string str1, string str2);
bool lessString (string str1, string str2);
bool moreString (string str1, string str2);
void copyString (string strDest, string strSrc);
void printString (string str);
char* bufferString (string str);