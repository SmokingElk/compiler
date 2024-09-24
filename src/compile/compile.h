#pragma once
#include <stdio.h>
#include "../config.h"
#include "../data_structures/queue/queue.h"
#include "../data_structures/map/map.h"
#include "./lexeme/lexeme.h"
#include "./compileError/compileError.h"
#include "./lexer/lexer.h"
#include "./syntaxAnalyzer/syntaxAnalyzer.h"
#include "./type/type.h"
#include "./nameTable/nameTable.h"
#include "./binCodeGen/binCodeGen.h"

typedef struct {
    PriorityNameTable nameTable;
    Map typeTable;
    StrSet functions;
    string currentFunction;
    CompileError compileError;
    Queue lexemesInfix;
    Queue lexemesPostfix;
    Stack scopesOpenings;
    Lexer lexer;
    SyntaxAnalyzer analyzer;
    BinCodeGen codeGen;
} _Compiler, *Compiler;

Compiler createCompiler ();
void deleteCompiler (Compiler compiler);
CompileError getCompileError (Compiler compiler);
bool compile (Compiler compiler, FILE *in, FILE *out);