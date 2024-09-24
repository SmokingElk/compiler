#include <stdio.h>
#include "./compile/compile.h"

typedef enum {
    EXIT_ERROR_NONE = 0,
    EXIT_ERROR_OPENING_FILE = 2,
    EXIT_ERROR_USAGE = 3,
} ExitCodes;

void printUsageHelp (int argc) {
    printf("Error: expected 2 arguments, got: %d.\n", argc);
    printf("Usage: compiler input_filename output_filename\n");
}

void printFileOpeningError (char *filename) {
    printf("Error while opening file %s.\n", filename);
}

void printError (CompileError error) {
    switch (getCErrorType(error)) {
        case CERROR_NONE:
            break;
        case CERROR_LEXICAL:
            printf("Lexical error. ");
            printLexicalError((LexicalError)error);
            break;
        case CERROR_SYNTAX:
            printf("Syntax error. ");
            printSyntaxError((SyntaxError)error);
            break;
        case CERROR_TYPE:
            printf("Type error. ");
            printTypeError((TypeError)error);
            break;
        case CERROR_NAME:
            printf("Name error. ");
            printNameError((NameError)error);
            break;
        default:
            break;
    }
}

int main (int argc, char *argv[]) {
    if (argc < 3) {
        printUsageHelp(argc);
        return EXIT_ERROR_USAGE;
    }   

    Compiler compiler = createCompiler();

    FILE *inputFile = fopen(argv[1], "r");

    if (inputFile == NULL) {
        printFileOpeningError(argv[1]);
        return EXIT_ERROR_OPENING_FILE;
    }
    
    FILE *outputFile = fopen(argv[2], "w");

    if (outputFile == NULL) {
        printFileOpeningError(argv[2]);
        fclose(inputFile);
        return EXIT_ERROR_OPENING_FILE;
    }

    bool compilationSuccess = compile(compiler, inputFile, outputFile);

    fclose(inputFile);
    fclose(outputFile);

    if (!compilationSuccess) {
        printError(getCompileError(compiler));

        // удалить выходной файл
    }

    deleteCompiler(compiler);

    return EXIT_ERROR_NONE;
}
