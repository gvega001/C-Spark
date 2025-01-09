#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

// Define token types
typedef enum {
    TOKEN_KEYWORD, TOKEN_IDENTIFIER, TOKEN_OPERATOR,
    TOKEN_LITERAL, TOKEN_SYMBOL, TOKEN_STRING,
    TOKEN_COMMENT, TOKEN_EOF
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value; // Dynamically allocated value
    int line;
    int column;
} Token;

// Function declarations
int is_keyword(const char* str);
Token* resize_tokens(Token* tokens, int* capacity);
void free_tokens(Token* tokens, int count);
void handle_unterminated_string(int line, int column, Token* tokens, int count);
void handle_unterminated_comment(int line, int column, Token* tokens, int count);
void tokenize_identifier(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void tokenize_operator(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void tokenize_literal(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void tokenize_string(const char* code, int* i, int* column, int line, Token* tokens, int* count);
Token* tokenize(const char* code, int* token_count);
void print_tokens(Token* tokens, int count);

#endif // LEXER_H
#pragma once
