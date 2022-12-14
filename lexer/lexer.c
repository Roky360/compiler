#include "lexer.h"
#include "../config/globals.h"
#include "../logging/logging.h"
#include "../io/io.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

Lexer *init_lexer(char *src) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer)
        log_error(LEXER, "Cant allocate memory for lexer.");

    lexer->src = src;
    lexer->src_len = strlen(src);
    lexer->idx = 0;
    lexer->row = 0;
    lexer->col = 0;
    lexer->c = src[0];

    return lexer;
}

void lexer_dispose(Lexer *lexer) {
    free(lexer);
}

void lexer_forward(Lexer *lexer) {
    (lexer->idx)++;
    (lexer->col)++;
    lexer->c = lexer->src[lexer->idx];
}

char lexer_peek(Lexer *lexer, int amount) {
    // clamp amount between 0 and src_len
    return lexer->src[MAX(0, MIN(lexer->src_len, lexer->idx + amount))];
}

void lexer_skip_whitespace(Lexer *lexer) {
    while (lexer->c == ' ' || lexer->c == '\t' || lexer->c == '\n') {
        if (lexer->c == '\n') // new line
        {
            (lexer->row)++;
            lexer->col = -1;
        }
        lexer_forward(lexer);
    }
}

Token *lexer_parse_id_token(Lexer *lexer) {
    char *val = calloc(1, sizeof(char));
    if (!val)
        log_error(LEXER, "Cant allocate memory for token value_expr.");

    while (isalnum(lexer->c) || lexer->c == '_') {
        val = realloc(val, (strlen(val) + 2) * sizeof(char));
        strcat(val, (char[]) {lexer->c, 0});
        lexer_forward(lexer);
    }

    if (!strcmp(val, "void"))
        return init_token(val, VOID_KEYWORD);
    else if (!strcmp(val, "int"))
        return init_token(val, INT_KEYWORD);
    else if (!strcmp(val, "string"))
        return init_token(val, STRING_KEYWORD);
    else if (!strcmp(val, "bool"))
        return init_token(val, BOOL_KEYWORD);
    else if (!strcmp(val, "char"))
        return init_token(val, CHAR_KEYWORD);
    else if (!strcmp(val, "return"))
        return init_token(val, RETURN_KEYWORD);
    else if (!strcmp(val, "func"))
        return init_token(val, FUNC_KEYWORD);
    else if (!strcmp(val, "if"))
        return init_token(val, IF_KEYWORD);
    else if (!strcmp(val, "else"))
        return init_token(val, ELSE_KEYWORD);

    return init_token(val, ID);
}

Token *lexer_parse_int_token(Lexer *lexer) {
    char *val = calloc(1, sizeof(char));
    if (!val)
        log_error(LEXER, "Cant allocate memory for token value_expr.");

    while (isdigit(lexer->c)) {
        val = realloc(val, (strlen(val) + 2) * sizeof(char));
        strcat(val, (char[]) {lexer->c, 0});
        lexer_forward(lexer);
    }

    return init_token(val, INT);
}

char *get_escape_character(Lexer *lexer) {
    char next = lexer_peek(lexer, 1);
    switch (next) {
        case 'n':
            return (char[]) {'\n', 0};
        case 't':
            return (char[]) {'\t', 0};
        case 'r':
            return (char[]) {'\r', 0};
        case 'b':
            return (char[]) {'\b', 0};
        case '"':
            return (char[]) {'"', 0};
        default:
            return (char[]) {lexer->c, next, 0};
    }
}

Token *lexer_parse_string_token(Lexer *lexer) {
    size_t val_len = 1;
    char *val = calloc(val_len, sizeof(char));
    if (!val)
        log_error(LEXER, "Cant allocate memory for token value_expr.");

    lexer_forward(lexer);
    while (lexer->c != '"') {
        val = realloc(val, ++val_len);
        if (lexer->c == '\\') { // support escape characters
            strcat(val, get_escape_character(lexer));
            lexer_forward(lexer);
        } else {
            strcat(val, (char[]) {lexer->c, 0});
        }
        lexer_forward(lexer);
    }

    return init_token(val, STRING);
}

void lexer_skip_one_line_comment(Lexer *lexer) {
    while (lexer->c != 0 && lexer->c != '\n') {
        lexer_forward(lexer);
    }
}

void lexer_skip_multi_line_comment(Lexer *lexer) {
    unsigned int row = lexer->row, col = lexer->col, idx = lexer->idx;
    lexer_forward(lexer);
    lexer_forward(lexer);
    while (!(lexer->c == '-' && lexer_peek(lexer, 1) == '/')) {
        if (lexer->c == 0) {
            lexer->row = row;
            lexer->col = col;
            lexer->idx = idx;
            throw_exception_with_trace(LEXER, lexer, "Comment unclosed at end of file");
        }
        lexer_forward(lexer);
    }
    lexer_forward(lexer);
    lexer_forward(lexer);
}

Token *lexer_next_token(Lexer *lexer) {
    Token *t;
    char *errorMsg;
    char *currC;

    lexer_skip_whitespace(lexer);

    if (isalpha(lexer->c))
        t = lexer_parse_id_token(lexer);
    else if (isdigit(lexer->c))
        t = lexer_parse_int_token(lexer);
    else {
        alsprintf(&currC, "%c", lexer->c);

        switch (lexer->c) {
            case '(':
                t = init_token(currC, L_PARENTHESES);
                break;
            case ')':
                t = init_token(currC, R_PARENTHESES);
                break;
            case '{':
                t = init_token(currC, L_CURLY_BRACE);
                break;
            case '}':
                t = init_token(currC, R_CURLY_BRACE);
                break;
            case '[':
                t = init_token(currC, L_SQUARE_BRACKET);
                break;
            case ']':
                t = init_token(currC, R_SQUARE_BRACKET);
                break;
            case ';':
                t = init_token(currC, SEMICOLON);
                break;
            case ',':
                t = init_token(currC, COMMA);
                break;
            case ':':
                t = init_token(currC, COLON);
                break;
            case '=':
                if (lexer_peek(lexer, 1) == '=') {
                    t = init_token("==", EQUALS);
                    lexer_forward(lexer);
                } else
                    t = init_token(currC, ASSIGNMENT);
                break;
            case '>':
                if (lexer_peek(lexer, 1) == '=') {
                    t = init_token(">=", GRATER_EQUAL);
                    lexer_forward(lexer);
                } else
                    t = init_token(currC, GRATER_THAN);
                break;
            case '<':
                if (lexer_peek(lexer, 1) == '=') {
                    t = init_token("<=", LOWER_EQUAL);
                    lexer_forward(lexer);
                } else
                    t = init_token(currC, LOWER_THAN);
                break;
            case '/':
                if (lexer_peek(lexer, 1) == '/') {
                    lexer_skip_one_line_comment(lexer);
                } else if (lexer_peek(lexer, 1) == '-') {
                    lexer_skip_multi_line_comment(lexer);
                } else {
                    t = init_token(currC, DIVIDE);
                    break;
                }
                t = lexer_next_token(lexer);
                break;
            case '"':
                t = lexer_parse_string_token(lexer);
                break;
            case '-':
                if (lexer_peek(lexer, 1) == '>') {
                    t = init_token("->", ARROW);
                    lexer_forward(lexer);
                } else if (lexer_peek(lexer, 1) == '-') {
                    t = init_token("--", DEC);
                    lexer_forward(lexer);
                } else {
                    t = init_token(currC, SUB);
                }
                break;
            case '+':
                if (lexer_peek(lexer, 1) == '+') {
                    t = init_token("++", INC);
                    lexer_forward(lexer);
                } else {
                    t = init_token(currC, ADD);
                }
                break;
            case '*':
                t = init_token(currC, MUL);
                break;
            case 0: // EOF
                t = init_token(currC, EOF_TOKEN);
                break;
            default:
                alsprintf(&errorMsg, "Unknown token '%c'", lexer->c);
                throw_exception_with_trace(LEXER, lexer, errorMsg);
                break;
        }
        lexer_forward(lexer);
    }

    return t;
}
