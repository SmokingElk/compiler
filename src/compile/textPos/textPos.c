#include "textPos.h"

TextPos createTextPos (int row, int col) {
    TextPos pos = {row, col};
    return pos;
}

void printTextPos (TextPos pos) {
    printf("Line: %d, Column: %d", pos.row + 1, pos.col + 1);
}
