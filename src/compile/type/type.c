#include "type.h"

Type createType (int size, List components) {
    Type type = (Type)malloc(sizeof(_Type));
    type->size = size;
    type->components = components;
    return type;
}

Type createComponentlessType (int size) {
    return createType(size, createList());
}

void deleteType (Type type) {
    for (ListIter it = beginList(type->components); iterNotEquals(it, endList(type->components)); it = iterNext(it)) {
        deleteString(iterFetch(string, it));
    }

    deleteList(type->components);

    free(type);
}

ListIter componentFirstType (Type type) {
    return beginList(type->components);
}

bool isLastComponentType (Type type, ListIter it) {
    return iterEquals(it, endList(type->components));
}

int sizeType (Type type) {
    return type->size;
}

MEM_MODE memModeType (Type type) {
    switch (type->size) {
        case 1: return MEM_WORD_8;
        case 2: return MEM_WORD_16;
        default: return MEM_WORD_16;
    }
}

void printType (Type type) {
    printf("Type<");

    ListIter componentIter = componentFirstType(type);

    if (!isLastComponentType(type, componentIter)) {
        printString(iterFetch(string, componentIter));
        componentIter = iterNext(componentIter);
    }

    for (; !isLastComponentType(type, componentIter); componentIter = iterNext(componentIter)) {
        printf(", ");
        printString(iterFetch(string, componentIter));
    }

    printf(">(size: %d)", type->size);
}

TypeError createTypeError (TextPos pos, string content, TypeErrorSubtype subtype) {
    TypeError err = (TypeError)malloc(sizeof(_TypeError));
    err->parent.type = CERROR_TYPE;
    err->pos = pos;
    err->content = createString(bufferString(content));
    err->subtype = subtype;
    return err;
}

void printTypeError (TypeError error) {
    switch (error->subtype) {
        case TYPE_ERROR_MULTIPLE_TYPES_IN_DECLARATION:
            printf("Multiple types in declaration: "); printString(error->content); printf(". ");
            break;
        
        default:
            break;
    }

    printTextPos(error->pos);
}

