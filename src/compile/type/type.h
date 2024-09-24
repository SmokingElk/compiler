#pragma once 
#include "../../data_structures/dstring/dstring.h"
#include "../../data_structures/linkedList/linkedList.h"
#include "../instruction/instruction.h"
#include "../textPos/textPos.h"
#include "../compileError/compileError.h"
#include <stdlib.h>

typedef enum {
    TYPE_ERROR_NONE = 0,
    TYPE_ERROR_MULTIPLE_TYPES_IN_DECLARATION,
} TypeErrorSubtype;

typedef struct {
    _CompileError parent;
    TextPos pos;
    string content;
    TypeErrorSubtype subtype;
} _TypeError, *TypeError;

typedef struct {
    int size;
    List components;
} _Type, *Type;

Type createType (int size, List components);
Type createComponentlessType (int size);
void deleteType (Type type);
ListIter componentFirstType (Type type);
bool isLastComponentType (Type type, ListIter it);
int sizeType (Type type);
MEM_MODE memModeType (Type type);
void printType (Type type);

TypeError createTypeError (TextPos pos, string content, TypeErrorSubtype subtype);
void printTypeError (TypeError error);