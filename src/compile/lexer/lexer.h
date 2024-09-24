#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../../config.h"
#include "../../data_structures/queue/queue.h"
#include "../../data_structures/strset/strset.h"
#include "../lexeme/lexeme.h"
#include "../textPos/textPos.h"
#include "../compileError/compileError.h"

typedef enum {
    LEXER_STATE_NONE = 0,
    READ_LETTER,
    READ_BEGIN,
    READ_NUM,
    READ_STR,
    READ_NAME_OR_KEYWORD,
    READ_OPERATOR,
    READ_END,

    READ_ERROR_UNKNOWN_OPERATOR = -1,
    READ_ERROR_NAME_STARTS_WITH_DIGIT = -2,
    READ_ERROR_UNCLOSED_STRING = -3,
} LexerState;

typedef struct {
    StrSet operatorsSet;
    StrSet keywordsSet;
} LexSets;

typedef char LexemeContentBuffer[MAX_LEXEME_LENGTH];

typedef struct {
    _CompileError parent;
    LexerState subtype;
    TextPos pos;
    LexemeContentBuffer content;
} _LexicalError, *LexicalError;

typedef struct {
    LexerState state;

    char letter;
    LexemeContentBuffer contentBuffer;
    int contentLength;

    int letterRow;
    int letterCol;
    TextPos lexemePos;

    LexSets sets;
} _Lexer, *Lexer;

Lexer createLexer ();
// считывает содержимое файла in до терминатора выражения, разбивает на лексемы и добавляет их в очередь out
bool getLexemes (Lexer lexer, FILE *in, Queue out);
LexicalError getErrorLexer (Lexer lexer);
void printLexicalError (LexicalError errorObject);
void deleteLexer (Lexer lexer);