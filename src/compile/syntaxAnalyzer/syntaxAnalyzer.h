#pragma once
#include "../../config.h"
#include "../../data_structures/map/map.h"
#include "../../data_structures/dstring/dstring.h"
#include "../../data_structures/queue/queue.h"
#include "../../data_structures/stack/stack.h"
#include "../../data_structures/strset/strset.h"
#include "../compileError/compileError.h"
#include "../textPos/textPos.h"
#include "../lexeme/lexeme.h"
#include "../opProps/opProps.h"
#include <stdbool.h>
#include <stdlib.h>

typedef enum {
    TILL_END,
    TILL_RECURSIVE_CLOSE,
} TranslateTill;

typedef enum {
    SYNTAX_ERROR_NONE = 0,
    SYNTAX_ERROR_UNCLOSED_BRACKET,
    SYNTAX_ERROR_UNOPENED_BRACKET,
    SYNTAX_ERROR_UNCLOSED_BLOCK,
    SYNTAX_ERROR_UNOPENED_BLOCK,
    SYNTAX_ERROR_UNEXPECTED_LEXEME,
    SYNTAX_ERROR_TOO_FEW_OPERANDS,
} SyntaxErrorSubtype;

typedef struct {
    _CompileError parent;
    TextPos pos;
    string content;
    SyntaxErrorSubtype subtype;
} _SyntaxError, *SyntaxError;

typedef struct {
    Map opPropsMap;
    SyntaxError error;
} _SyntaxAnalyzer, *SyntaxAnalyzer;

SyntaxAnalyzer createSyntaxAnalyzer ();
void deleteSyntaxAnalyzer (SyntaxAnalyzer analyzer);
bool translateToPostfix (SyntaxAnalyzer analyzer, Queue infix, Queue postfix, StrSet functions, TranslateTill till);
SyntaxError getSyntaxError (SyntaxAnalyzer analazer);

OpPriority getLexOpPriority (SyntaxAnalyzer analyzer, Lexeme lexeme);
OpArity getLexOpArity (SyntaxAnalyzer analyzer, Lexeme lexeme);

SyntaxError createSyntaxError (TextPos pos, string content, SyntaxErrorSubtype subtype);
void printSyntaxError (SyntaxError errorObject);
