#pragma once
#include <stdio.h>

typedef struct {
    int row;
    int col;
} TextPos;

TextPos createTextPos (int row, int col);
void printTextPos (TextPos pos);