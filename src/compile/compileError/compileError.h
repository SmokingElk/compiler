#pragma once
#include <stdlib.h>

typedef enum {
    CERROR_NONE = 0,
    CERROR_LEXICAL,
    CERROR_SYNTAX,
    CERROR_TYPE,
    CERROR_NAME,
} CErrorType;

typedef struct {
    CErrorType type;
} _CompileError, *CompileError;

typedef struct {
    _CompileError parent;
} _CompileErrorNone, *CompileErrorNone;

CErrorType getCErrorType (CompileError errorObject);
CompileErrorNone createCompileErrorNone ();