#include "opProps.h"

OpProps createOpProps (OpPriority priority, OpArity arity, OpAssociativity associativity) {
    OpProps props = (OpProps)malloc(sizeof(_OpProps));
    props->priority = priority;
    props->arity = arity;
    props->associativity = associativity;
    return props;
}

void deleteOpProps (OpProps props) {
    free(props);
}

OpPriority opPriorityProp (OpProps props) {
    return props->priority;
}

OpArity opArityProp (OpProps props) {
    return props->arity;
}

OpAssociativity opAssociativityProp (OpProps props) {
    return props->associativity;
}
