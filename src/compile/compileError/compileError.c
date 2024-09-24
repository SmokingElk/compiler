#include "compileError.h"

CErrorType getCErrorType (CompileError errorObject) {
    return errorObject->type;
}

CompileErrorNone createCompileErrorNone () {
    CompileErrorNone error = (CompileErrorNone)malloc(sizeof(_CompileErrorNone));
    error->parent.type = CERROR_NONE;
    return error;
}
