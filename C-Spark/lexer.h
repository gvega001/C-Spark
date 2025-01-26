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
void handle_unterminated_string(int line, int column, const char* code, int position, Token* tokens, int count);
void handle_unterminated_comment(int line, int column, Token* tokens, int count);
void tokenize_identifier(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void tokenize_operator(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void tokenize_literal(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void tokenize_string(const char* code, int* i, int* column, int line, Token* tokens, int* count);
Token* tokenize(const char* code, int* token_count);
void summarize_errors(int error_count, int warning_count);
void handle_unknown_character(char c, int line, int column);
void tokenize_symbol(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void process_string_content(const char* code, int* i, int* column, char* buffer, int* j, int line, int start_column, int start_position, Token* tokens, int* count);

void tokenize_comment(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void handle_whitespace(const char* code, int* i, int* line, int* column);

int dispatch_tokenizer(const char* code, int* i, int* column, int line, Token* tokens, int* count);
void add_eof_token(Token* tokens, int* count, int line, int column);
void handle_unknown_character_and_advance(const char* code, int* i, int* column, int line);
void process_multi_line_comment(const char* code, int* i, int* column, int* line, Token* tokens, int* count);
void process_single_line_comment(const char* code, int* i, int* column);

#endif // LEXER_H
#pragma once
