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

// Keywords for the language
const char* keywords[] = { "let", "print", "if", "else", "for"};

// Function to check if a string is a keyword
int is_keyword(const char* str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Function to dynamically resize the token array
Token* resize_tokens(Token* tokens, int* capacity) {
    int new_capacity = *capacity * 2;
    Token* new_tokens = safe_realloc(tokens, new_capacity * sizeof(Token));
    *capacity = new_capacity;
    return new_tokens;
}

// Function to free token array
void free_tokens(Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}

// Function to handle unterminated string literals
void handle_unterminated_string(int line, int column, Token* tokens, int count) {
    fprintf(stderr, "Error: Unterminated string literal at line %d, column %d\n", line, column);
    free_tokens(tokens, count);
    exit(1);
}

// Function to handle unterminated comments
void handle_unterminated_comment(int line, int column, Token* tokens, int count) {
    fprintf(stderr, "Error: Unterminated multi-line comment at line %d, column %d\n", line, column);
    free_tokens(tokens, count);
    exit(1);
}

// Function to tokenize unknown characters gracefully
void handle_unknown_character(char character, int line, int column) {
    fprintf(stderr, "Warning: Unknown character '%c' at line %d, column %d. Skipping.\n", character, line, column);
}

// Function to tokenize keywords and identifiers
void tokenize_identifier(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };
    int j = 0, start_column = *column;
    while (isalnum(code[*i])) {
        buffer[j++] = code[(*i)++];
        (*column)++;
    }
    buffer[j] = '\0';

    if (is_keyword(buffer)) {
        tokens[(*count)++] = (Token){ TOKEN_KEYWORD, _strdup(buffer), line, start_column };
    }
    else {
        tokens[(*count)++] = (Token){ TOKEN_IDENTIFIER, _strdup(buffer), line, start_column };
    }
}

// Function to tokenize operators
void tokenize_operator(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[3] = { 0 };
    int start_column = *column;
    buffer[0] = code[*i];
    if ((code[*i] == '=' || code[*i] == '<' || code[*i] == '>' || code[*i] == '!' || code[*i] == '+' || code[*i] == '-') && code[*i + 1] == '=') {
        buffer[1] = code[*i + 1];
        (*i) += 2;
        (*column) += 2;
    }
    else if ((code[*i] == '&' || code[*i] == '|') && code[*i + 1] == code[*i]) {
        buffer[1] = code[*i + 1];
        (*i) += 2;
        (*column) += 2;
    }
    else {
        (*i)++;
        (*column)++;
    }
    tokens[(*count)++] = (Token){ TOKEN_OPERATOR, _strdup(buffer), line, start_column };
}

// Function to tokenize literals
void tokenize_literal(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };
    int j = 0, start_column = *column;

    if (code[*i] == '0' && (code[*i + 1] == 'x' || code[*i + 1] == 'b')) {
        buffer[j++] = code[(*i)++];
        buffer[j++] = code[(*i)++];
        (*column) += 2;

        while (isxdigit(code[*i]) || (buffer[1] == 'b' && (code[*i] == '0' || code[*i] == '1'))) {
            buffer[j++] = code[(*i)++];
            (*column)++;
        }
    }
    else {
        int has_decimal = 0;
        while (isdigit(code[*i]) || code[*i] == '.') {
            if (code[*i] == '.') {
                if (has_decimal) break; // Prevent multiple dots
                has_decimal = 1;
            }
            buffer[j++] = code[(*i)++];
            (*column)++;
        }
    }

    buffer[j] = '\0';
    tokens[(*count)++] = (Token){ TOKEN_LITERAL, _strdup(buffer), line, start_column };
}

// Function to tokenize string literals
void tokenize_string(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };
    int j = 0, start_column = *column;
    (*i)++; // Skip opening quote
    (*column)++;
    while (code[*i] != '"' && code[*i] != '\0') {
        if (code[*i] == '\\') {
            (*i)++;
            (*column)++;
            switch (code[*i]) {
            case 'n': buffer[j++] = '\n'; break;
            case 't': buffer[j++] = '\t'; break;
            case '\\': buffer[j++] = '\\'; break;
            case '"': buffer[j++] = '"'; break;
            default: buffer[j++] = code[*i]; break; // Unknown escape
            }
        }
        else {
            buffer[j++] = code[(*i)];
        }
        (*i)++;
        (*column)++;
    }
    if (code[*i] == '"') {
        (*i)++; // Skip closing quote
        (*column)++;
    }
    else {
        handle_unterminated_string(line, start_column, tokens, *count);
    }
    buffer[j] = '\0';
    tokens[(*count)++] = (Token){ TOKEN_STRING, _strdup(buffer), line, start_column };
}

// Function to tokenize the input
Token* tokenize(const char* code, int* token_count) {
    Token* tokens = malloc(100 * sizeof(Token));  // Allocate memory for tokens
    if (!tokens) {
        fprintf(stderr, "Error: Initial memory allocation failed\n");
        exit(1);
    }
    int capacity = 100;  // Initial capacity
    int i = 0, count = 0, line = 1, column = 1;

    while (code[i] != '\0') {
        if (isspace(code[i])) {
            if (code[i] == '\n') {
                line++;
                column = 1;
            }
            else {
                column++;
            }
            i++;  // Skip whitespace
            continue;
        }

        // Resize tokens array if needed
        if (count >= capacity) {
            Token* resized = resize_tokens(tokens, &capacity);
            if (!resized) {
                free_tokens(tokens, count);
                return NULL;
            }
            tokens = resized;
        }

        // Keywords and Identifiers
        if (isalpha(code[i])) {
            tokenize_identifier(code, &i, &column, line, tokens, &count);
        }
        // Operators and Compound Operators
        else if (strchr("=+*-<>!&|", code[i])) {
            tokenize_operator(code, &i, &column, line, tokens, &count);
        }
        // Literals (numbers, hex, binary)
        else if (isdigit(code[i]) || (code[i] == '0' && (code[i + 1] == 'x' || code[i + 1] == 'b'))) {
            tokenize_literal(code, &i, &column, line, tokens, &count);
        }
        // String Literals
        else if (code[i] == '"') {
            tokenize_string(code, &i, &column, line, tokens, &count);
        }
        // Symbols
        else if (strchr(";(){}", code[i])) { // Added braces
            char buffer[2] = { code[i], '\0' };
            tokens[count++] = (Token){ TOKEN_SYMBOL, _strdup(buffer), line, column };
            i++;
            column++;
        }
        // Comments (single-line and multi-line)
        else if (code[i] == '/' && code[i + 1] == '/') {
            while (code[i] != '\n' && code[i] != '\0') i++;
            line++;
            column = 1;
        }
        else if (code[i] == '/' && code[i + 1] == '*') {
            i += 2;
            column += 2;
            while (!(code[i] == '*' && code[i + 1] == '/') && code[i] != '\0') {
                if (code[i] == '\n') {
                    line++;
                    column = 1;
                }
                else {
                    column++;
                }
                i++;
            }
            if (code[i] == '*' && code[i + 1] == '/') {
                i += 2;
                column += 2;
            }
            else {
                handle_unterminated_comment(line, column, tokens, count);
            }
        }
        // Unknown characters
        else {
            handle_unknown_character(code[i], line, column);
            i++;
            column++;
        }
    }

    // End of input
    tokens[count++] = (Token){ TOKEN_EOF, _strdup(""), line, column };
    *token_count = count;
    return tokens;
}

