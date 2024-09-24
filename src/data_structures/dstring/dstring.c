#include "dstring.h"

string createString (char* str) {
    string newStr = (string)malloc(sizeof(_string));
    newStr->buffer = (char*)malloc(sizeof(char) * (strlen(str) + 1));
    strcpy(newStr->buffer, str);
    return newStr;
}

string createStringCopy (string str) {
    return createString(bufferString(str));
}

void deleteString (string str) {
    free(str->buffer);
    free(str);
}

int lenString (string str) {
    return strlen(str->buffer);
}

bool equalString (string str1, string str2) {
    return strcmp(str1->buffer, str2->buffer) == 0;
}

bool lessString (string str1, string str2) {
    return strcmp(str1->buffer, str2->buffer) < 0;
}

bool moreString (string str1, string str2) {
    return strcmp(str1->buffer, str2->buffer) > 0;
}

void copyString (string strDest, string strSrc) {
    deleteString(strDest);

    strDest->buffer = (char*)malloc(sizeof(char) * (lenString(strSrc) + 1));
    strcpy(strDest->buffer, strSrc->buffer);
}

void printString (string str) {
    printf("%s", str->buffer);
}

char* bufferString (string str) {
    return str->buffer;
}