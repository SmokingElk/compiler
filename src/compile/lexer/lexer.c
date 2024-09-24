#include "lexer.h"

LexicalError _createLexicalError (LexerState subtype, char *content, TextPos pos);

void _resetLexer (Lexer lexer);
void _readLetterLexer (Lexer lexer, FILE *in);
void _addLetterToContentLexer (Lexer lexer, char letter);
LexerState _nextStateLexer (Lexer lexer, FILE *in, Queue out);

LexerState _lexerStateReadLetter (Lexer lexer, FILE *in, Queue out);
LexerState _lexerStateReadBegin (Lexer lexer, FILE *in, Queue out);
LexerState _lexerStateReadNum (Lexer lexer, FILE *in, Queue out);
LexerState _lexerStateReadStr (Lexer lexer, FILE *in, Queue out);
LexerState _lexerStateReadNameOrKeyword (Lexer lexer, FILE *in, Queue out);
LexerState _lexerStateReadOperator (Lexer lexer, FILE *in, Queue out);

LexSets createLexSets () {
    LexSets sets;

    sets.keywordsSet = createStrSet();

    addStrSet(sets.keywordsSet, KEYWORD_ELSE);
    addStrSet(sets.keywordsSet, KEYWORD_IF);
    addStrSet(sets.keywordsSet, KEYWORD_WHILE);
    addStrSet(sets.keywordsSet, KEYWORD_FUNCTION);
    addStrSet(sets.keywordsSet, KEYWORD_RETURN);

    sets.operatorsSet = createStrSet();

    addStrSet(sets.operatorsSet, OPERATOR_ASSIGNMENT);
    
    addStrSet(sets.operatorsSet, OPERATOR_PLUS);
    addStrSet(sets.operatorsSet, OPERATOR_MINUS);
    addStrSet(sets.operatorsSet, OPERATOR_MULT);
    addStrSet(sets.operatorsSet, OPERATOR_DIV);

    addStrSet(sets.operatorsSet, OPERATOR_EQUAL);
    addStrSet(sets.operatorsSet, OPERATOR_NOT_EQUAL);
    addStrSet(sets.operatorsSet, OPERATOR_LESS);
    addStrSet(sets.operatorsSet, OPERATOR_LESS_EQUAL);
    addStrSet(sets.operatorsSet, OPERATOR_MORE);
    addStrSet(sets.operatorsSet, OPERATOR_MORE_EQUAL);
    
    addStrSet(sets.operatorsSet, OPERATOR_AND);
    addStrSet(sets.operatorsSet, OPERATOR_OR);
    addStrSet(sets.operatorsSet, OPERATOR_NOT);

    addStrSet(sets.operatorsSet, OPERATOR_BITWISE_OR);

    addStrSet(sets.operatorsSet, OPERATOR_PRINT);

    return sets;
}

void deleteLexSets (LexSets lexSets) {
    deleteStrSet(lexSets.keywordsSet);
    deleteStrSet(lexSets.operatorsSet);
}

bool isDigit (char letter) {
    return '0' <= letter && letter <= '9';
} 

bool isAlpha (char letter) {
    return ('a' <= letter && letter <= 'z') || ('A' <= letter && letter <= 'Z');
}

bool isNameLetter (char letter) {
    return isDigit(letter) || isAlpha(letter) || letter == '_';
}

bool isSpace (char letter) {
    return letter == ' ' || letter == '\t' || letter == '\n';
}

bool isOp (char letter) {
    return strchr("+-*/=<>!&|~@", letter) != NULL; 
}

LexicalError _createLexicalError (LexerState subtype, char *content, TextPos pos) {
    LexicalError error = (LexicalError)malloc(sizeof(_LexicalError));
    error->parent.type = CERROR_LEXICAL;
    error->subtype = subtype;
    strcpy(error->content, content);
    error->pos = pos;
    return error;
}

void printLexicalError (LexicalError errorObject) {
    switch (errorObject->subtype) {
        case READ_ERROR_NAME_STARTS_WITH_DIGIT:
            printf("Name can not starts with digit: %s. ", errorObject->content);
            break;
        case READ_ERROR_UNCLOSED_STRING:
            printf("Unclosed string: %s. ", errorObject->content);
            break;
        case READ_ERROR_UNKNOWN_OPERATOR:
            printf("Unknown operator: %s. ", errorObject->content);
            break;
        default:
            break;
    }

    printTextPos(errorObject->pos);
}

Lexer createLexer () {
    Lexer res = (Lexer)malloc(sizeof(_Lexer));
    _resetLexer(res);

    res->letterRow = 0;
    res->letterCol = -1;
    res->sets = createLexSets();
    
    return res;
}

bool getLexemes (Lexer lexer, FILE *in, Queue out) {
    _resetLexer(lexer);

    while (lexer->state != READ_END && lexer->state > 0) {
        lexer->state = _nextStateLexer(lexer, in, out);
    }

    return lexer->state == READ_END;    
}

LexicalError getErrorLexer (Lexer lexer) {
    if (lexer->state >= 0) return NULL;
    return _createLexicalError(lexer->state, lexer->contentBuffer, lexer->lexemePos);
}

void deleteLexer (Lexer lexer) {
    deleteLexSets(lexer->sets);
    free(lexer);
}

void _resetLexer (Lexer lexer) {
    lexer->state = READ_LETTER;
    lexer->contentLength = 0;
}

void _readLetterLexer (Lexer lexer, FILE *in) {
    lexer->letter = fgetc(in);
    
    lexer->letterCol++;
    if (lexer->letter == '\n') {
        lexer->letterCol = -1;
        lexer->letterRow++;
    }
}

void _addLetterToContentLexer (Lexer lexer, char letter) {
    lexer->contentBuffer[lexer->contentLength++] = letter;
    lexer->contentBuffer[lexer->contentLength] = '\0';
}

LexerState _nextStateLexer (Lexer lexer, FILE *in, Queue out) {
    switch (lexer->state) {
        case READ_LETTER:
            return _lexerStateReadLetter(lexer, in, out);

        case READ_BEGIN:
            return _lexerStateReadBegin(lexer, in, out);

        case READ_NUM:
            return _lexerStateReadNum(lexer, in, out);
        
        case READ_STR:
            return _lexerStateReadStr(lexer, in, out);

        case READ_NAME_OR_KEYWORD:
            return _lexerStateReadNameOrKeyword(lexer, in, out);

        case READ_OPERATOR:
            return _lexerStateReadOperator(lexer, in, out);
        
        default:
            return LEXER_STATE_NONE;
    }
}

LexerState _lexerStateReadLetter (Lexer lexer, FILE *in, Queue out) {
    _readLetterLexer(lexer, in);
    return READ_BEGIN;
}

LexerState _lexerStateReadBegin (Lexer lexer, FILE *in, Queue out) {
    lexer->lexemePos = createTextPos(lexer->letterRow, lexer->letterCol);

    if (isSpace(lexer->letter)) return READ_LETTER;

    if (lexer->letter == ARG_SEP) {
        _addLetterToContentLexer(lexer, ARG_SEP);
        pushQueue(out, createLexeme(lexer->contentBuffer, LEX_ARG_SEP, lexer->lexemePos));
        lexer->contentLength = 0;
        return READ_LETTER;
    }
    
    if (lexer->letter == EXPR_OPEN_BRACKET) {
        _addLetterToContentLexer(lexer, EXPR_OPEN_BRACKET);
        pushQueue(out, createLexeme(lexer->contentBuffer, LEX_EXPR_OPEN_BRACKET, lexer->lexemePos));
        lexer->contentLength = 0;
        return READ_LETTER;
    }

    if (lexer->letter == EXPR_CLOSE_BRACKET) {
        _addLetterToContentLexer(lexer, EXPR_CLOSE_BRACKET);
        pushQueue(out, createLexeme(lexer->contentBuffer, LEX_EXPR_CLOSE_BRACKET, lexer->lexemePos));
        lexer->contentLength = 0;
        return READ_LETTER;
    }

    if (lexer->letter == BLOCK_OPEN_BRACKET) {
        _addLetterToContentLexer(lexer, BLOCK_OPEN_BRACKET);
        pushQueue(out, createLexeme(lexer->contentBuffer, LEX_BLOCK_OPEN_BRACKET, lexer->lexemePos));
        lexer->contentLength = 0;
        return READ_LETTER;
    }

    if (lexer->letter == BLOCK_CLOSE_BRACKET) {
        _addLetterToContentLexer(lexer, BLOCK_CLOSE_BRACKET);
        pushQueue(out, createLexeme(lexer->contentBuffer, LEX_BLOCK_CLOSE_BRACKET, lexer->lexemePos));
        lexer->contentLength = 0;
        return READ_LETTER;
    }

    if (isNameLetter(lexer->letter)) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return isDigit(lexer->letter) ? READ_NUM : READ_NAME_OR_KEYWORD;
    }

    if (lexer->letter == STR_QUOTES) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return READ_STR;
    }

    if (isOp(lexer->letter)) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return READ_OPERATOR;
    }

    if (lexer->letter == EXPR_TERMINATOR || feof(in)) return READ_END;
    return LEXER_STATE_NONE;
}

LexerState _lexerStateReadNum (Lexer lexer, FILE *in, Queue out) {
    _readLetterLexer(lexer, in);

    if (isDigit(lexer->letter)) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return READ_NUM;
    }

    if (isAlpha(lexer->letter)) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return READ_ERROR_NAME_STARTS_WITH_DIGIT;
    }

    pushQueue(out, createLexeme(lexer->contentBuffer, LEX_NUMBER, lexer->lexemePos));
    lexer->contentLength = 0;
    return READ_BEGIN;
}

LexerState _lexerStateReadStr (Lexer lexer, FILE *in, Queue out) {
    _readLetterLexer(lexer, in);

    if (lexer->letter == EXPR_TERMINATOR || (lexer->letter != STR_QUOTES && feof(in))) {
        return READ_ERROR_UNCLOSED_STRING;
    }

    _addLetterToContentLexer(lexer, lexer->letter);

    if (lexer->letter != STR_QUOTES) return READ_STR;

    pushQueue(out, createLexeme(lexer->contentBuffer, LEX_NUMBER, lexer->lexemePos));
    lexer->contentLength = 0;
    return READ_LETTER;
}

LexerState _lexerStateReadNameOrKeyword (Lexer lexer, FILE *in, Queue out) {
    _readLetterLexer(lexer, in);

    if (isNameLetter(lexer->letter)) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return READ_NAME_OR_KEYWORD;
    }

    bool isKeyword = hasStrSet(lexer->sets.keywordsSet, lexer->contentBuffer);

    pushQueue(out, createLexeme(lexer->contentBuffer, isKeyword ? LEX_KEYWORD : LEX_NAME, lexer->lexemePos));
    lexer->contentLength = 0;
    return READ_BEGIN;
}

LexerState _lexerStateReadOperator (Lexer lexer, FILE *in, Queue out) {
    _readLetterLexer(lexer, in);

    if (isOp(lexer->letter)) {
        _addLetterToContentLexer(lexer, lexer->letter);
        return READ_OPERATOR;
    } 

    if (!hasStrSet(lexer->sets.operatorsSet, lexer->contentBuffer)) {
        return READ_ERROR_UNKNOWN_OPERATOR;
    }

    pushQueue(out, createLexeme(lexer->contentBuffer, LEX_OPERATOR, lexer->lexemePos));
    lexer->contentLength = 0;
    return READ_BEGIN;
}
