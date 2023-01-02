#include "token.h"
#include <stdio.h>
#include <stdlib.h>

Token *init_token(char *value, TokenType type) {
    Token *token = malloc(sizeof(Token));
    if (!token) {
        printf("Cant alllocate memory for token\n");
        exit(1);
    }
    token->value = value;
    token->type = type;

    return token;
}

void token_dispose(Token *token) {
    free(token->value);
    free(token);
}

char *token_type_to_str(TokenType type) {
    switch (type) {
        case INT:
            return "<INT>";
        case STRING:
            return "<STRING>";
        case ID:
            return "<ID>";
        case VOID_KEYWORD:
            return "<VOID_KEYWORD>";
        case INT_KEYWORD:
            return "<INT_KEYWORD>";
        case STRING_KEYWORD:
            return "<STRING_KEYWORD>";
        case BOOL_KEYWORD:
            return "<BOOL_KEYWORD>";
        case CHAR_KEYWORD:
            return "<CHAR_KEYWORD>";
        case IF_KEYWORD:
            return "<IF_KEYWORD>";
        case ELSE_KEYWORD:
            return "<ELSE_KEYWORD>";
        case L_PARENTHESES:
            return "<LEFT_PARENTHESES>";
        case R_PARENTHESES:
            return "<RIGHT_PARENTHESES>";
        case L_CURLY_BRACE:
            return "<LEFT_BRACE>";
        case R_CURLY_BRACE:
            return "<RIGHT_BRACE>";
        case RETURN_KEYWORD:
            return "<RETURN_KEYWORD>";
        case FUNC_KEYWORD:
            return "<FUNC_KEYWORD>";
        case SEMICOLON:
            return "<SEMICOLON>";
        case COMMA:
            return "<COMMA>";
        case COLON:
            return "<COLON>";
        case ASSIGNMENT:
            return "<ASSIGNMENT>";
        case EQUALS:
            return "<EQUALS>";
        case GRATER_THAN:
            return "<GRATER_THAN>";
        case GRATER_EQUAL:
            return "<GRATER_EQUAL>";
        case LOWER_THAN:
            return "<LOWER_THAN>";
        case LOWER_EQUAL:
            return "<LOWER_EQUAL>";
        case ADD:
            return "<ADD>";
        case SUB:
            return "<SUBTRACT>";
        case MUL:
            return "<MULTIPLY>";
        case DIVIDE:
            return "<DIVIDE>";
        case ARROW:
            return "<ARROW>";
        case DEC:
            return "<DEC>";
        case INC:
            return "<INC>";
        case EOF_TOKEN:
            return "<EOF>";
        default:
            return "<Unknown-token>";
    }
}
