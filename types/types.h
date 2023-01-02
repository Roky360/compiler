#ifndef INFINITY_COMPILER_TYPES_H
#define INFINITY_COMPILER_TYPES_H

#include "../list/list.h"
#include "../token/token.h"

typedef enum {
    TYPE_VOID,
    TYPE_INT,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_BOOL,
    TYPE_DOUBLE,
} DataType;

typedef union {
    int integer_value;
    char char_value;
    char *string_value;
    int bool_value;
    double double_value;
    void *void_value; // if void - initialize this to NULL
} Value;

typedef struct {
    DataType type;
    Value value;
} LiteralValue;

/*
An `Expression` represents an expression with or without variables.
Like: 5+7 or x*2-3
*/
typedef struct {
    List *tokens;
    LiteralValue *value;
    int contains_variables; // contains variables, like: 2 * x + 3
    // without variables: 5 - 8 / 4
} Expression;

LiteralValue *init_literal_value(DataType type, Value value);

void literal_value_dispose(LiteralValue *value);

Expression *init_expression_p();

Expression init_expression();

void expression_dispose(Expression *expr);

DataType token_type_to_data_type(TokenType type);

#endif //INFINITY_COMPILER_TYPES_H
