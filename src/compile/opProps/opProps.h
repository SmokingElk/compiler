#pragma once
#include <stdlib.h>

typedef enum {
    PRIORITY_NONE = 0,
    PRIORITY_LOWEST = 1,

    PRIORITY_BITWISE_OR = 11,

    PRIORITY_BOOL_ADD = 21,
    PRIORITY_BOOL_MULT = 22,
    PRIORITY_BOOL_NOT = 23,

    PRIORITY_CMP = 29,

    PRIORITY_ADDITIVE = 31,
    PRIORITY_MULTIPLICATIVE = 32,
    
    PRIORITY_HIGHEST = 127,
} OpPriority;

typedef enum {
    ARITY_NONE = 0,
    ARITY_UNARY = 1,
    ARITY_BINARY = 2,
} OpArity;

typedef enum {
    ASSOC_NONE = 0,
    ASSOC_LEFT,
    ASSOC_RIGHT,
} OpAssociativity;

typedef struct {
    OpPriority priority;
    OpArity arity;
    OpAssociativity associativity;
} _OpProps, *OpProps;

OpProps createOpProps (OpPriority priority, OpArity arity, OpAssociativity associativity);
void deleteOpProps (OpProps props);
OpPriority opPriorityProp (OpProps props);
OpArity opArityProp (OpProps props);
OpAssociativity opAssociativityProp (OpProps props);
