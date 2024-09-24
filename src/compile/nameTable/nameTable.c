#include "nameTable.h"

#define SECTION_TOP_LIMIT 10

Name createName (int relativeAddr, char *typeName);
void nameDestructror (void *object);

void _setTopNameTable (NameTable table, int top);
void _makeBase (NameTable table);
bool _isBase (NameTable table);

NameTable createNameTable () {
    NameTable table = (NameTable)malloc(sizeof(_NameTable));
    table->top = 0;
    table->names = createMap(SECTION_TOP_LIMIT);
    table->isBase = false;
    return table;
}

void deleteNameTable (NameTable table) {
    deleteMap(table->names, nameDestructror);
    free(table);
}

bool nameInTable (NameTable table, char *name) {
    return hasMap(table->names, name);
}

Name getNameFromTable (NameTable table, char *name) {
    return getMap(Name, table->names, name);
}

int getTopNameTable (NameTable table) {
    return table->top;
}

NameErrorType addNameToTable (NameTable table, char *name, char *typeName, Map typeTable) {
    if (!hasMap(typeTable, typeName)) return NAME_ADDING_ERROR_UNKNOWN_TYPE;
    if (nameInTable(table, name)) return NAME_ADDING_ERROR_ALREADY_EXISTS;

    int size = sizeType(getMap(Type, typeTable, typeName));
    table->top -= size;

    Name addingName = createName(table->top + 1, typeName);
    setMap(table->names, name, addingName);

    return NAME_ADDING_SUCCESS;
}

PriorityNameTable createPriorityNameTable () {
    PriorityNameTable table = (PriorityNameTable)malloc(sizeof(_PriorityNameTable));
    table->sections = createList();
    return table;
}

void deletePriorityNameTable (PriorityNameTable table) {
    for (ListIter it = beginList(table->sections); iterNotEquals(it, endList(table->sections)); it = iterNext(it)) {
        deleteNameTable(iterFetch(NameTable, it));
    }
    
    deleteList(table->sections);
    free(table);
}

bool nameInPriorityTable (PriorityNameTable table, char *name) {
    for (ListIter it = beginList(table->sections); iterNotEquals(it, endList(table->sections)); it = iterNext(it)) {
        if (nameInTable(iterFetch(NameTable, it), name)) return true;
        if (_isBase(iterFetch(NameTable, it))) break;
    }
    
    return false;
}

Name getNameFromPriorityTable (PriorityNameTable table, char *name) {
    for (ListIter it = beginList(table->sections); iterNotEquals(it, endList(table->sections)); it = iterNext(it)) {
        Name nameObject = getNameFromTable(iterFetch(NameTable, it), name);
        if (nameObject != NULL) return nameObject;
        if (_isBase(iterFetch(NameTable, it))) break;
    }

    return NULL;
}

NameErrorType addNameToPriorityTable (PriorityNameTable table, char *name, char *typeName, Map typeTable) {
    if (isEmptyList(table->sections)) return NAME_ADDING_ERROR_NO_SECTIONS;
    return addNameToTable(iterFetch(NameTable, beginList(table->sections)), name, typeName, typeTable);
}

void addBaseSectionToPriorityTable (PriorityNameTable table) {
    NameTable section = createNameTable();
    _makeBase(section);
    insertToList(table->sections, endList(table->sections), section);
}

bool addLinkedSectionToPriorityTable (PriorityNameTable table) {
    if (isEmptyList(table->sections)) return false;
    NameTable section = createNameTable();

    // сцепка по адресу верхушки
    _setTopNameTable(section, getTopNameTable(iterFetch(NameTable, beginList(table->sections))));

    insertToList(table->sections, endList(table->sections), section);
    return true;
}

bool popTopSectionFromPriorityTable (PriorityNameTable table) {
    if (isEmptyList(table->sections)) return false;
    deleteNameTable(iterFetch(NameTable, beginList(table->sections)));
    deleteFromList(table->sections, beginList(table->sections));
    return true;
}

int getTopPriorityTable (PriorityNameTable table) {
    if (isEmptyList(table->sections)) return 0; 
    return getTopNameTable(iterFetch(NameTable, beginList(table->sections)));
}

void _setTopNameTable (NameTable table, int top) {
    table->top = top;
}

void _makeBase (NameTable table) {
    table->isBase = true;
}

bool _isBase (NameTable table) {
    return table->isBase;
}

word relativeAddrName (Name name) {
    return name->relativeAddr;
}

char* typeOfName (Name name) {
    return name->typeName;
}

Name createName (int relativeAddr, char *typeName) {
    Name name = (Name)malloc(sizeof(_Name));
    name->relativeAddr = relativeAddr;
    name->typeName = (char*)malloc(sizeof(char) * strlen(typeName));
    strcpy(name->typeName, typeName);

    return name;
}

void nameDestructror (void *object) {
    free((Name)object);
}

NameError createNameError (TextPos pos, string content, NameErrorType subtype) {
    NameError error = (NameError)malloc(sizeof(_NameError));
    error->parent.type = CERROR_NAME;
    error->pos = pos;
    error->content = createString(bufferString(content));
    error->subtype = subtype;
    return error;
}

void printNameError (NameError error) {
    switch (error->subtype) {
        case NAME_ADDING_ERROR_ALREADY_EXISTS:
            printf("Name already exists: "); printString(error->content); printf(". ");
            break;

        case NAME_ADDING_ERROR_UNKNOWN_TYPE:
            printf("Unknown name type: "); printString(error->content); printf(". ");
            break;

        case NAME_ERROR_UNDECLARATED:
            printf("Name "); printString(error->content); printf(" is not declarated in this scope. ");
            break;

        default:
            break;
    }

    printTextPos(error->pos);
} 
