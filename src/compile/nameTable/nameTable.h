#pragma once
#include "../../data_structures/map/map.h"
#include "../../data_structures/linkedList/linkedList.h"
#include "../../data_structures/dstring/dstring.h"
#include "../instruction/instruction.h"
#include "../type/type.h"
#include "../textPos/textPos.h"
#include "../compileError/compileError.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

typedef enum {
    NAME_ADDING_SUCCESS,
    NAME_ADDING_ERROR_ALREADY_EXISTS,
    NAME_ADDING_ERROR_UNKNOWN_TYPE,
    NAME_ERROR_UNDECLARATED,
    NAME_ADDING_ERROR_NO_SECTIONS,
} NameErrorType;

typedef struct {
    word relativeAddr;
    char *typeName;
} _Name, *Name;

typedef struct {
    _CompileError parent;
    TextPos pos;
    string content;
    NameErrorType subtype;
} _NameError, *NameError;

typedef struct {
    word top;
    Map names;
    bool isBase;
} _NameTable, *NameTable;

typedef struct {
    List sections;
} _PriorityNameTable, *PriorityNameTable;

NameTable createNameTable ();
void deleteNameTable (NameTable table);
bool nameInTable (NameTable table, char *name);
Name getNameFromTable (NameTable table, char *name);
NameErrorType addNameToTable (NameTable table, char *name, char *typeName, Map typeTable);
int getTopNameTable (NameTable table);

PriorityNameTable createPriorityNameTable ();
void deletePriorityNameTable (PriorityNameTable table);
bool nameInPriorityTable (PriorityNameTable table, char *name);
Name getNameFromPriorityTable (PriorityNameTable table, char *name);
NameErrorType addNameToPriorityTable (PriorityNameTable table, char *name, char *typeName, Map typeTable);
void addBaseSectionToPriorityTable (PriorityNameTable table);
bool addLinkedSectionToPriorityTable (PriorityNameTable table);
bool popTopSectionFromPriorityTable (PriorityNameTable table);
int getTopPriorityTable (PriorityNameTable table);

word relativeAddrName (Name name);
char* typeOfName (Name name);

NameError createNameError (TextPos pos, string content, NameErrorType subtype);
void printNameError (NameError error);