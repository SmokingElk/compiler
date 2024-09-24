#include "syntaxAnalyzer.h"

#define OPERATORS_TOP_LIMIT 100

void _opPropsDestructor (void *object);
bool _isFunction (StrSet functions, Lexeme lexeme);
Map _createOperationsPropsMap ();

SyntaxError createSyntaxError (TextPos pos, string content, SyntaxErrorSubtype subtype);

SyntaxAnalyzer createSyntaxAnalyzer () {
    SyntaxAnalyzer analazer = (SyntaxAnalyzer)malloc(sizeof(_SyntaxAnalyzer));
    analazer->opPropsMap = _createOperationsPropsMap();
    analazer->error = NULL;
    return analazer;
}

void deleteSyntaxAnalyzer (SyntaxAnalyzer analyzer) {
    deleteMap(analyzer->opPropsMap, _opPropsDestructor);
}

bool translateToPostfix (SyntaxAnalyzer analyzer, Queue infix, Queue postfix, StrSet functions, TranslateTill till) {
    // Dijkstra's sorting station algorithm
    int recursiveDepth = 0;
    Stack opStack = createStack(0);

    while (!isEmptyQueue(infix)) {
        Lexeme topLexeme = topQueue(Lexeme, infix);
        LexType topLexType = typeLexeme(topLexeme);

        if (topLexType == LEX_NUMBER || topLexType == LEX_STRING) {
            pushQueue(postfix, topLexeme);
        }

        if (topLexType == LEX_NAME) {
            if (_isFunction(functions, topLexeme)) {
                pushStack(opStack, topLexeme);
            } else {
                pushQueue(postfix, topLexeme);
            }
        }

        if (topLexType == LEX_EXPR_OPEN_BRACKET) {
            pushStack(opStack, topLexeme);
            recursiveDepth++;
        }

        if (topLexType == LEX_EXPR_CLOSE_BRACKET) {
            recursiveDepth--;

            // выход по завершению скобки
            if (recursiveDepth == -1 && till == TILL_RECURSIVE_CLOSE) {
                break;
            }

            while (!isEmptyStack(opStack) && typeLexeme(topStack(Lexeme, opStack)) != LEX_EXPR_OPEN_BRACKET) {
                pushQueue(postfix, popStack(Lexeme, opStack));
            }
            
            if (isEmptyStack(opStack)) {
                analyzer->error = createSyntaxError(
                    posLexeme(topLexeme), 
                    contentLexeme(topLexeme), 
                    SYNTAX_ERROR_UNOPENED_BRACKET
                );

                while (!isEmptyStack(opStack)) deleteLexeme(popStack(Lexeme, opStack));
                deleteStack(opStack);
                
                return false;
            } 
            
            deleteLexeme(popStack(Lexeme, opStack));
            deleteLexeme(topLexeme);
        }

        if (topLexType == LEX_ARG_SEP) {
            while (!isEmptyStack(opStack) && typeLexeme(topStack(Lexeme, opStack)) != LEX_EXPR_OPEN_BRACKET) {
                pushQueue(postfix, popStack(Lexeme, opStack));
            }
            
            if (isEmptyStack(opStack)) {
                analyzer->error = createSyntaxError(
                    posLexeme(topLexeme), 
                    contentLexeme(topLexeme), 
                    SYNTAX_ERROR_UNEXPECTED_LEXEME
                );

                while (!isEmptyStack(opStack)) deleteLexeme(popStack(Lexeme, opStack));
                deleteStack(opStack);
                
                return false;
            } 

            deleteLexeme(topLexeme);
        }

        if (topLexType == LEX_OPERATOR) {
            OpPriority topPriority = getLexOpPriority(analyzer, topLexeme);

            // пока на вершине стека более приоритетная операция - переложить ее на выход 
            // функция имеет наивысший приоритет
            while (!isEmptyStack(opStack) &&
                (typeLexeme(topStack(Lexeme, opStack)) == LEX_OPERATOR && 
                topPriority <= getLexOpPriority(analyzer, topStack(Lexeme, opStack)) ||
                typeLexeme(topStack(Lexeme, opStack)) == LEX_NAME)) {
                
                pushQueue(postfix, popStack(Lexeme, opStack));
            }

            pushStack(opStack, topLexeme);
        }

        popQueue(Lexeme, infix);
    }

    while (!isEmptyStack(opStack)) {
        if (typeLexeme(topStack(Lexeme, opStack)) == LEX_EXPR_OPEN_BRACKET) {
            analyzer->error = createSyntaxError(
                posLexeme(topStack(Lexeme, opStack)), 
                contentLexeme(topStack(Lexeme, opStack)), 
                SYNTAX_ERROR_UNCLOSED_BRACKET
            );

            while (!isEmptyStack(opStack)) deleteLexeme(popStack(Lexeme, opStack));
            deleteStack(opStack);
            
            return false;
        }   

        pushQueue(postfix, popStack(Lexeme, opStack));
    }
    
    deleteStack(opStack);
    return true;
}

SyntaxError getSyntaxError (SyntaxAnalyzer analazer) {
    return analazer->error;
}

SyntaxError createSyntaxError (TextPos pos, string content, SyntaxErrorSubtype subtype) {
    SyntaxError error = (SyntaxError)malloc(sizeof(_SyntaxError));
    error->parent.type = CERROR_SYNTAX;
    error->pos = pos;
    error->content = createString(bufferString(content));
    error->subtype = subtype;
} 

void printSyntaxError (SyntaxError errorObject) {
    switch (errorObject->subtype) {
        case SYNTAX_ERROR_UNCLOSED_BRACKET:
            printf("Unclosed bracket: "); printString(errorObject->content); printf(". ");
            break;

        case SYNTAX_ERROR_UNOPENED_BRACKET:
            printf("Unopened bracket: "); printString(errorObject->content); printf(". ");
            break;

        case SYNTAX_ERROR_UNCLOSED_BLOCK:
            printf("Unclosed block: "); printString(errorObject->content); printf(". ");
            break;

        case SYNTAX_ERROR_UNOPENED_BLOCK:
            printf("Unopened block: "); printString(errorObject->content); printf(". ");
            break;

        case SYNTAX_ERROR_UNEXPECTED_LEXEME:
            printf("Unexpected token: "); printString(errorObject->content); printf(". ");
            break;

        case SYNTAX_ERROR_TOO_FEW_OPERANDS:
            printf("Too few operands for operator: "); printString(errorObject->content); printf(". ");
            break;

        default:
            break;
    }

    printTextPos(errorObject->pos);
}

void _opPropsDestructor (void *object) {
    deleteOpProps((OpProps)object);
}

Map _createOperationsPropsMap () {
    Map map = createMap(OPERATORS_TOP_LIMIT);
    
    setMap(map, OPERATOR_ASSIGNMENT, createOpProps(PRIORITY_LOWEST, ARITY_BINARY, ASSOC_RIGHT));

    setMap(map, OPERATOR_OR, createOpProps(PRIORITY_BOOL_ADD, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_AND, createOpProps(PRIORITY_BOOL_MULT, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_NOT, createOpProps(PRIORITY_BOOL_NOT, ARITY_UNARY, ASSOC_LEFT));

    setMap(map, OPERATOR_PLUS, createOpProps(PRIORITY_ADDITIVE, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_MINUS, createOpProps(PRIORITY_ADDITIVE, ARITY_BINARY, ASSOC_LEFT));

    setMap(map, OPERATOR_MULT, createOpProps(PRIORITY_MULTIPLICATIVE, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_DIV, createOpProps(PRIORITY_MULTIPLICATIVE, ARITY_BINARY, ASSOC_LEFT));

    setMap(map, OPERATOR_EQUAL, createOpProps(PRIORITY_CMP, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_NOT_EQUAL, createOpProps(PRIORITY_CMP, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_LESS, createOpProps(PRIORITY_CMP, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_LESS_EQUAL, createOpProps(PRIORITY_CMP, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_MORE, createOpProps(PRIORITY_CMP, ARITY_BINARY, ASSOC_LEFT));
    setMap(map, OPERATOR_MORE_EQUAL, createOpProps(PRIORITY_CMP, ARITY_BINARY, ASSOC_LEFT));

    setMap(map, OPERATOR_BITWISE_OR, createOpProps(PRIORITY_BITWISE_OR, ARITY_BINARY, ASSOC_LEFT));

    setMap(map, OPERATOR_PRINT, createOpProps(PRIORITY_LOWEST, ARITY_UNARY, ASSOC_LEFT));
    
    return map;
}

OpPriority getLexOpPriority (SyntaxAnalyzer analyzer, Lexeme lexeme) {
    return opPriorityProp(getMap(OpProps, analyzer->opPropsMap, bufferString(contentLexeme(lexeme))));
}

OpArity getLexOpArity (SyntaxAnalyzer analyzer, Lexeme lexeme) {
    return opArityProp(getMap(OpProps, analyzer->opPropsMap, bufferString(contentLexeme(lexeme))));
}

// определить по множеству имен функций является ли имя именем функции
bool _isFunction (StrSet functions, Lexeme lexeme) {
    return hasStrSet(functions, bufferString(contentLexeme(lexeme)));
}