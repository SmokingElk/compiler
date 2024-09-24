#pragma once
#include <stdlib.h>
#include <stdio.h>
#include "../instruction/instruction.h"
#include "../../data_structures/dstring/dstring.h"
#include "../textPos/textPos.h"

typedef enum {
    LEX_NUMBER,
    LEX_STRING,
    LEX_NAME,
    LEX_OPERATOR,
    LEX_ARG_SEP,
    LEX_EXPR_OPEN_BRACKET,
    LEX_EXPR_CLOSE_BRACKET,
    LEX_BLOCK_OPEN_BRACKET,
    LEX_BLOCK_CLOSE_BRACKET,
    LEX_KEYWORD,
} LexType;

typedef struct {
    string content;
    LexType type;
    TextPos textPos;
} _Lexeme, *Lexeme;

Lexeme createLexeme (char *content, LexType type, TextPos textPos);
void deleteLexeme (Lexeme lexeme);
string contentLexeme (Lexeme lexeme);
TextPos posLexeme (Lexeme lexeme);
LexType typeLexeme (Lexeme lexeme);
word readNumLexeme (Lexeme lexeme);
void printLexeme (Lexeme lexeme);