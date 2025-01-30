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
    TOKEN_COMMENT, TOKEN_EOF, TOKEN_COLON // Added TOKEN_COLON
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value; // Dynamically allocated value
    int line;
    int column;
} Token;

// Public API functions
int is_keyword(const char* str);                      // Check if a string is a keyword
Token* resize_tokens(Token* tokens, int* capacity);   // Resize the token array
void free_tokens(Token* tokens, int count);           // Free the tokens array
Token* tokenize(const char* code, int* token_count);  // Main tokenize function
void summarize_errors(int error_count, int warning_count); // Summarize tokenization errors and warnings

#endif // LEXER_H
