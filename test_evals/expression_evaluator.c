#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_TOKEN_LEN 100

typedef enum
{
    NUMBER,
    OPERATOR,
    PAREN
} TokenType;

typedef struct
{
    TokenType type;
    union
    {
        double number;
        char op;
        char paren;
    } value;
} Token;

typedef struct
{
    Token *tokens;
    int size;
    int capacity;
} TokenArray;

void token_array_init(TokenArray *array)
{
    array->size = 0;
    array->capacity = 8;
    array->tokens = malloc(sizeof(Token) * array->capacity);
}

void token_array_free(TokenArray *array)
{
    free(array->tokens);
}

void token_array_push(TokenArray *array, Token token)
{
    if (array->size == array->capacity)
    {
        array->capacity *= 2;
        array->tokens = realloc(array->tokens, sizeof(Token) * array->capacity);
    }
    array->tokens[array->size++] = token;
}

int token_array_is_empty(TokenArray *array)
{
    return array->size == 0;
}

Token token_array_pop(TokenArray *array)
{
    return array->tokens[--array->size];
}

Token token_array_top(TokenArray *array)
{
    return array->tokens[array->size - 1];
}

int get_precedence(char op)
{
    switch (op)
    {
    case '+':
    case '-':
        return 1;
    case '*':
    case '/':
    case '%':
        return 2;
    case '^':
        return 3;
    default:
        return -1;
    }
}

int is_right_associative(char op)
{
    return op == '^';
}

void parse_tokens(const char *expression, TokenArray *tokens)
{
    const char *p = expression;
    char current_token[MAX_TOKEN_LEN];
    int current_token_len = 0;

    while (*p)
    {
        if (isspace(*p))
        {
            // Ignore whitespace
        }
        else if (isdigit(*p) || *p == '.')
        {
            // Number token
            current_token[current_token_len++] = *p;
        }
        else if (strchr("+-*/^%%()", *p))
        {
            // Operator or parenthesis token
            if (current_token_len > 0)
            {
                // Flush current number token
                current_token[current_token_len] = '\0';
                Token token;
                token.type = NUMBER;
                token.value.number = atof(current_token);
                token_array_push(tokens, token);
                current_token_len = 0;
            }
            Token token;
            token.type = (*p == '(' || *p == ')') ? PAREN : OPERATOR;
            token.value.op = *p;
            token_array_push(tokens, token);
        }
        else
        {
            fprintf(stderr, "Invalid character: %c\n", *p);
            exit(1);
        }
        p++;
    }

    if (current_token_len > 0)
    {
        // Flush final number token
        current_token[current_token_len] = '\0';
        Token token;
        token.type = NUMBER;
        token.value.number = atof(current_token);
        token_array_push(tokens, token);
    }
}

void infix_to_postfix(TokenArray *infix, TokenArray *postfix)
{
    TokenArray operator_stack;
    token_array_init(&operator_stack);

    for (int i = 0; i < infix->size; i++)
    {
        Token token = infix->tokens[i];
        if (token.type == NUMBER)
        {
            token_array_push(postfix, token);
        }
        else if (token.type == OPERATOR)
        {
            while (!token_array_is_empty(&operator_stack) &&
                   token_array_top(&operator_stack).type == OPERATOR &&
                   (is_right_associative(token.value.op) ? get_precedence(token.value.op) < get_precedence(token_array_top(&operator_stack).value.op) : get_precedence(token.value.op) <= get_precedence(token_array_top(&operator_stack).value.op)))
            {
                token_array_push(postfix, token_array_pop(&operator_stack));
            }
            token_array_push(&operator_stack, token);
        }
        else if (token.type == PAREN)
        {
            if (token.value.paren == '(')
            {
                token_array_push(&operator_stack, token);
            }
            else
            {
                while (token_array_top(&operator_stack).type != PAREN)
                {
                    token_array_push(postfix, token_array_pop(&operator_stack));
                }
                token_array_pop(&operator_stack);
            }
        }
    }

    while (!token_array_is_empty(&operator_stack))
    {
        token_array_push(postfix, token_array_pop(&operator_stack));
    }

    token_array_free(&operator_stack);
}

double evaluate_postfix(TokenArray *postfix)
{
    TokenArray stack;
    token_array_init(&stack);

    for (int i = 0; i < postfix->size; i++)
    {
        Token token = postfix->tokens[i];
        if (token.type == NUMBER)
        {
            token_array_push(&stack, token);
        }
        else if (token.type == OPERATOR)
        {
            double b = token_array_pop(&stack).value.number;
            double a = token_array_pop(&stack).value.number;
            double result;
            switch (token.value.op)
            {
            case '+':
                result = a + b;
                break;
            case '-':
                result = a - b;
                break;
            case '*':
                result = a * b;
                break;
            case '/':
                result = a / b;
                break;
            case '^':
                result = pow(a, b);
                break;
            case '%':
                result = (int)a % (int)b;
                break;
            default:
                fprintf(stderr, "Invalid operator: %c\n", token.value.op);
                exit(1);
            }
            Token result_token;
            result_token.type = NUMBER;
            result_token.value.number = result;
            token_array_push(&stack, result_token);
        }
    }

    double result = token_array_pop(&stack).value.number;
    if (!token_array_is_empty(&stack))
    {
        fprintf(stderr, "Invalid expression\n");
        exit(1);
    }

    token_array_free(&stack);
    return result;
}

double evaluate(const char *expression)
{
    TokenArray infix, postfix;
    token_array_init(&infix);
    token_array_init(&postfix);
    parse_tokens(expression, &infix);
    infix_to_postfix(&infix, &postfix);
    double result = evaluate_postfix(&postfix);
    token_array_free(&infix);
    token_array_free(&postfix);
    return result;
}

/*
***
Shunting yard algorithm
***
*/

int main(int argc, char **argv)
{
    // if (argc != 2)
    // {
    //     fprintf(stderr, "Usage: %s EXPRESSION\n", argv[0]);
    //     return 1;
    // }
    char *expression = calloc(100, sizeof(char));
    gets(expression);
    printf("%f\n", evaluate(expression));
    return 0;
}
