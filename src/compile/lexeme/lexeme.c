#include "lexeme.h"

Lexeme createLexeme (char *content, LexType type, TextPos textPos) {
    Lexeme lexeme = (Lexeme)malloc(sizeof(_Lexeme));
    lexeme->content = createString(content);
    lexeme->type = type;
    lexeme->textPos = textPos;
    return lexeme;
}

void deleteLexeme (Lexeme lexeme) {
    deleteString(lexeme->content);
    free(lexeme);
}

string contentLexeme (Lexeme lexeme) {
    return lexeme->content;
}

TextPos posLexeme (Lexeme lexeme) {
    return lexeme->textPos;
}

LexType typeLexeme (Lexeme lexeme) {
    return lexeme->type;
}

word readNumLexeme (Lexeme lexeme) {
    if (lexeme->type != LEX_NUMBER) return 0;

    word num = 0;

    char *buffer = bufferString(lexeme->content);

    for (int i = 0; buffer[i] != '\0'; i++) {
        // схема Горнера
        num = num * 10 + (int)buffer[i] - (int)'0';
    }

    return num;
}

void printLexeme (Lexeme lexeme) {
    printf("Lexeme<");

    switch (typeLexeme(lexeme)) {
        case LEX_NUMBER              : printf("LEX_NUMBER"); break;
        case LEX_STRING              : printf("LEX_STRING"); break;
        case LEX_NAME                : printf("LEX_NAME"); break;
        case LEX_OPERATOR            : printf("LEX_OPERATOR"); break;
        case LEX_ARG_SEP             : printf("LEX_ARG_SEP"); break;
        case LEX_EXPR_OPEN_BRACKET   : printf("LEX_EXPR_OPEN_BRACKET"); break;
        case LEX_EXPR_CLOSE_BRACKET  : printf("LEX_EXPR_CLOSE_BRACKET"); break;
        case LEX_BLOCK_OPEN_BRACKET  : printf("LEX_BLOCK_OPEN_BRACKET"); break;
        case LEX_BLOCK_CLOSE_BRACKET : printf("LEX_BLOCK_CLOSE_BRACKET"); break;
        case LEX_KEYWORD             : printf("LEX_KEYWORD"); break;   
    }

    printf(">(\""); printString(contentLexeme(lexeme)); printf("\"); ");
    printTextPos(lexeme->textPos);
}