#ifndef INFINITY_COMPILER_LEXER_H
#define INFINITY_COMPILER_LEXER_H

#include <stdlib.h>
#include "../token/token.h"

typedef struct LexerStruct {
    char *src;
    size_t src_len;
    char c;           // current character
    unsigned int idx; // index of current character
    unsigned int row; // line number     - for error reporting
    unsigned int col; // column number   - for error reporting
} Lexer;

Lexer *init_lexer(char *src);

void lexer_dispose(Lexer *lexer);

void lexer_forward(Lexer *lexer);

char lexer_peek(Lexer *lexer, int amount);

void lexer_skip_whitespace(Lexer *lexer);

Token *lexer_parse_id_token(Lexer *lexer);

Token *lexer_parse_int_token(Lexer *lexer);

char *get_escape_character(Lexer *lexer);

Token *lexer_parse_string_token(Lexer *lexer);

void lexer_skip_one_line_comment(Lexer *lexer);

void lexer_skip_multi_line_comment(Lexer *lexer);

Token *lexer_next_token(Lexer *lexer);

#endif //INFINITY_COMPILER_LEXER_H
