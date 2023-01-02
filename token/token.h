#ifndef INFINITY_COMPILER_TOKEN_H
#define INFINITY_COMPILER_TOKEN_H

typedef enum TokenType {
    /** Values */
    INT,    // integer constant value_expr
    STRING, // string value_expr
    ID,
    /** Keywords */
    VOID_KEYWORD,
    INT_KEYWORD,
    BOOL_KEYWORD,
    CHAR_KEYWORD,
    STRING_KEYWORD,
    RETURN_KEYWORD,
    FUNC_KEYWORD,
    IF_KEYWORD,
    ELSE_KEYWORD,
    /** Parentheses */
    L_PARENTHESES,     // (
    R_PARENTHESES,     // )
    L_CURLY_BRACE,    // {
    R_CURLY_BRACE,    // }
    L_SQUARE_BRACKET, // [
    R_SQUARE_BRACKET, // [
    /** Punctuation */
    SEMICOLON,
    COMMA,
    COLON,
    ASSIGNMENT, // =
    EQUALS,     // ==
    GRATER_THAN,
    LOWER_THAN,
    GRATER_EQUAL,
    LOWER_EQUAL,
    ADD,
    SUB,
    MUL,
    DIVIDE,
    ARROW,
    DEC,
    INC,

    EOF_TOKEN,
} TokenType;

typedef struct TokenStruct {
    TokenType type;
    char *value;
} Token;

Token *init_token(char *value, TokenType type);

void token_dispose(Token *token);

char *token_type_to_str(TokenType type);

#endif //INFINITY_COMPILER_TOKEN_H
