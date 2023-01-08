#include "parser.h"
#include "../config/globals.h"
#include "../lexer/lexer.h"
#include "../logging/logging.h"
#include "../io/io.h"
#include <stdio.h>

Parser *init_parser(Lexer *lexer) {
    Parser *parser = malloc(sizeof(Parser));
    if (!parser)
        log_error(PARSER, "Cant allocate memory for parser.");

    parser->lexer = lexer;
    parser->token = lexer_next_token(lexer);
    return parser;
}

void parser_dispose(Parser *parser) {
    lexer_dispose(parser->lexer);
    token_dispose(parser->token);
    free(parser);
}

void parser_handle_unexpected_token(Parser *parser, char *expectations) {
    char *errMsg;
    alsprintf(&errMsg, "Unexpected token: '%s'. Expecting '%s'", parser->token->value, expectations);
    throw_exception_with_trace(PARSER, parser->lexer, errMsg);
}

/*
Proceeds to the next token, while checking
that the current token is of the same type as the `type` parameter.
Returns the current token. parser->token gets the next token.
*/
Token *parser_forward(Parser *parser, TokenType type) {
    Token *currTok;

    if (parser->token->type != type) {
        parser_handle_unexpected_token(parser, token_type_to_str(type));
    }
    currTok = parser->token;
    parser->token = lexer_next_token(parser->lexer);

    return currTok;
}

/**
 * Moving forward with a list of expected tokens.
 * The `expectations` parameter will be displayed as error message
 * in case the current token doesn't satisfy any of the tokens in the `types` list.
 */
Token *parser_forward_with_list(Parser *parser, TokenType *types, size_t types_len, char *expectations) {
    int i;
    for (i = 0; i < types_len; i++) {
        if (parser->token->type == types[i])
            return parser_forward(parser, types[i]);
    }
    parser_handle_unexpected_token(parser, expectations);
    return NULL;
}

AstNode *parser_parse(Parser *parser) {
    return parser_parse_compound(parser);
}

AstNode *parser_parse_expression(Expression *expression) {
    AstNode *expr_node = init_ast(AST_EXPRESSION);

    switch (((Token *) expression->tokens->items[0])->type) {
        case STRING:
            expr_node->data.expression.value = init_literal_value(
                    TYPE_STRING,
                    (Value) {.string_value = ((Token *) expression->tokens->items[0])->value}
            );
            break;
        case SEMICOLON: // void
            expr_node->data.expression.value = init_literal_value(
                    TYPE_VOID,
                    (Value) {.void_value = NULL}
            );
            break;
        default:
            // parse expression
            // TODO: Link here parse expression function
            break;
    }

    return expr_node;
}

LiteralValue *get_default_literal_value(TokenType type) {
    char *errMsg;

    switch (type) {
        case INT_KEYWORD:
            return init_literal_value(TYPE_INT, (Value) {.integer_value = 0});
        case STRING_KEYWORD:
            return init_literal_value(TYPE_STRING, (Value) {.string_value = ""});
        case CHAR_KEYWORD:
            return init_literal_value(TYPE_INT, (Value) {.char_value = '\0'});
        case BOOL_KEYWORD:
            return init_literal_value(TYPE_BOOL, (Value) {.bool_value = 0});
        default:
            alsprintf(&errMsg, "Unsupported type '%s'", token_type_to_str(type));
            log_error(PARSER, errMsg);
            return NULL;
    }
}

void parser_get_tokens_until(Parser *parser, List *tokens, TokenType terminator) {
    while (parser->token->type != terminator) {
        list_push(tokens, parser_forward(parser, parser->token->type));
    }
    parser_forward(parser, terminator);
}

AstNode *parser_parse_compound(Parser *parser) {
    AstNode *root = init_ast(AST_COMPOUND);

    while (parser->token->type != EOF_TOKEN) {
        list_push(root->data.compound.children, parser_parse_statement(parser));
    }
    return root;
}

void parser_parse_block(Parser *parser, List *block) {
    parser_forward(parser, L_CURLY_BRACE);
    while (parser->token->type != R_CURLY_BRACE) {
        list_push(block, parser_parse_statement(parser));
    }
    parser_forward(parser, R_CURLY_BRACE);
}

AstNode *parser_parse_statement(Parser *parser) {
    char *errMsg;
    switch (parser->token->type) {
        case ID:
            return parser_parse_id(parser);
        case FUNC_KEYWORD:
            return parser_parse_function_definition(parser);
        case INT_KEYWORD:
        case CHAR_KEYWORD:
        case BOOL_KEYWORD:
        case STRING_KEYWORD:
            return parser_parse_var_declaration(parser);
        case IF_KEYWORD:
            return parser_parse_if_statement(parser);
        case RETURN_KEYWORD:
            return parser_parse_return_statement(parser);
        default:
            alsprintf(&errMsg, "Expected an expression, got %s", token_type_to_str(parser->token->type));
            throw_exception_with_trace(PARSER, parser->lexer, errMsg);
            return NULL;
    }
}

AstNode *parser_parse_id(Parser *parser) {
    Token *id_token = parser_forward(parser, ID);
    switch (parser->token->type) {
        case ASSIGNMENT:
            return parser_parse_assignment(parser, id_token);
        case L_PARENTHESES:
//            return parser_parse_function_call();
            break;
        case SEMICOLON:
            log_warning(parser->lexer, "Meaningless expression");
            parser_forward(parser, SEMICOLON);
            return init_ast(AST_NOOP);
        default:
            throw_exception_with_trace(PARSER, parser->lexer, "Expected");
            break;
    }
}

AstNode *parser_parse_var_declaration(Parser *parser) {
    AstNode *node, *value_expr;
    Token *var_type;
    Expression *expr;

    node = init_ast(AST_VARIABLE_DECLARATION);
    var_type = parser_forward_with_list(parser, data_types, data_types_len, "type definition");
    node->data.variable_declaration.var = init_variable(
            parser_forward(parser, ID)->value,
            init_literal_value(token_type_to_data_type(var_type->type), (Value) {})
    );

    // if value_expr is immediately assigned to variable
    if (parser->token->type == ASSIGNMENT) {
        expr = init_expression_p();
        parser_forward(parser, ASSIGNMENT);

        parser_get_tokens_until(parser, expr->tokens, SEMICOLON);
        node->data.variable_declaration.value = parser_parse_expression(expr);
    } else {
        // variable is initialized with default value_expr
        value_expr = init_ast(AST_EXPRESSION);
        parser_forward(parser, SEMICOLON);

        value_expr->data.expression.value = get_default_literal_value(var_type->type);
        value_expr->data.expression.contains_variables = 0;

        node->data.variable_declaration.value = value_expr;
    }

    return node;
}

AstNode *parser_parse_function_definition(Parser *parser) {
    char *errMsg;
    Variable *arg;
    DataType argType;
    AstNode *node = init_ast(AST_FUNCTION_DEFINITION);

    parser_forward(parser, FUNC_KEYWORD);

    // define function name
    node->data.function_definition.func_name = parser_forward(parser, ID)->value;

    // get arguments
    parser_forward(parser, L_PARENTHESES);
    while (parser->token->type != R_PARENTHESES) {
        // get arg type
        argType = token_type_to_data_type(parser->token->type);
        if ((int)argType == -1) // invalid type
        {
            alsprintf(&errMsg, "Expected argument type, got %s token.", token_type_to_str(parser->token->type));
            throw_exception_with_trace(PARSER, parser->lexer, errMsg);
        }
        parser_forward(parser, parser->token->type);
        // get arg name
        arg = init_variable(
                parser_forward(parser, ID)->value,
                init_literal_value(argType, (Value) {})
        );
        list_push(node->data.function_definition.args, arg);

        if (parser->token->type != R_PARENTHESES)
            parser_forward(parser, COMMA);
    }
    parser_forward(parser, R_PARENTHESES);

    // get return type
    parser_forward(parser, ARROW);
    node->data.function_definition.returnType = token_type_to_data_type(
            parser_forward_with_list(parser, data_types, data_types_len, "return type")
                    ->type);

    parser_parse_block(parser, node->data.function_definition.body);

    printf("name: %s\n", node->data.function_definition.func_name);
    printf("return type: %d\n", node->data.function_definition.returnType);
    for (int i = 0; i < node->data.function_definition.args->size; i++) {
        printf("var %s\n", ((Variable *) (node->data.function_definition.args->items[i]))->name);
    }
    AstNode *v1 = ((AstNode *) (node->data.function_definition.body->items[0]));
    AstNode *v2 = ((AstNode *) (node->data.function_definition.body->items[1]));
    AstNode *v3 = ((AstNode *) (node->data.function_definition.body->items[2]));

    return node;
}

AstNode *parser_parse_assignment(Parser *parser, Token *id_token) {
    AstNode *node = init_ast(AST_ASSIGNMENT);
    parser_forward(parser, ASSIGNMENT);
    node->data.assignment.dst_variable = id_token;
    node->data.assignment.expression = parser_parse_statement(parser);
    return node;
}

AstNode *parser_parse_if_statement(Parser *parser) {
    AstNode *node = init_ast(AST_IF_STATEMENT);

    parser_forward(parser, IF_KEYWORD);
    parser_forward(parser, L_PARENTHESES);
    // parse boolean expression
    parser_forward(parser, R_PARENTHESES);
    parser_parse_block(parser, node->data.if_statement.body_node);
    if (parser->token->type == ELSE_KEYWORD) {
        parser_forward(parser, ELSE_KEYWORD);
        if (parser->token->type == IF_KEYWORD)
            list_push(node->data.if_statement.else_node, parser_parse_if_statement(parser));
        else
            parser_parse_block(parser, node->data.if_statement.else_node);
    }

    return node;
}

AstNode *parser_parse_return_statement(Parser *parser) {
    AstNode *node = init_ast(AST_RETURN_STATEMENT);
    Expression *expr = init_expression_p();

    parser_forward(parser, RETURN_KEYWORD);

    parser_get_tokens_until(parser, expr->tokens, SEMICOLON);
    node->data.return_statement.value_expr = parser_parse_expression(expr);

    return node;
}
