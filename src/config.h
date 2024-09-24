#define PREPATCHED_FILE_PATH "./inner/linkless.bin"

#define MAX_LEXEME_LENGTH 50
#define EXPR_TERMINATOR ';'
#define ARG_SEP ','

#define EXPR_OPEN_BRACKET '('
#define EXPR_CLOSE_BRACKET ')'
#define BLOCK_OPEN_BRACKET '{'
#define BLOCK_CLOSE_BRACKET '}'
#define STR_QUOTES '"'

// операторы
#define OPERATOR_ASSIGNMENT "="

#define OPERATOR_PLUS "+"
#define OPERATOR_MINUS "-"
#define OPERATOR_MULT "*"
#define OPERATOR_DIV "/"

#define OPERATOR_EQUAL "=="
#define OPERATOR_NOT_EQUAL "!="
#define OPERATOR_LESS "<"
#define OPERATOR_LESS_EQUAL "<="
#define OPERATOR_MORE ">"
#define OPERATOR_MORE_EQUAL ">="

#define OPERATOR_AND "&&"
#define OPERATOR_OR "||"
#define OPERATOR_NOT "!"

#define OPERATOR_BITWISE_OR "|"

#define OPERATOR_PRINT "@"

// ключевые слова
#define KEYWORD_IF "if"
#define KEYWORD_ELSE "else"
#define KEYWORD_WHILE "while"
#define KEYWORD_FUNCTION "function"
#define KEYWORD_RETURN "return"

// базовые типы
#define BASE_TYPE_INT8 "int8"
#define BASE_TYPE_INT16 "int16"
#define BASE_TYPE_BOOL "bool"

#define NUM_LITERAL_TYPE BASE_TYPE_INT16

// имя точки входа
#define ENTRY_NAME "main"

// размер ОЗУ: 64 кб
#define RAM_CAPACITY 65536

// регистров процессора
#define REGS_COUNT 16