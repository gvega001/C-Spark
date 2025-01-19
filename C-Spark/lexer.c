// lexer.c
#include "lexer.h"

const char* keywords[] = { "let", "print", "if", "else", "for" };

// Check if a string is a keyword
int is_keyword(const char* str) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Dynamically resize token array
Token* resize_tokens(Token* tokens, int* capacity) {
    int new_capacity = *capacity * 2;
    Token* new_tokens = realloc(tokens, new_capacity * sizeof(Token));
    if (!new_tokens) {
        fprintf(stderr, "Error: Memory allocation failed during resizing\n");
        free_tokens(tokens, *capacity);
        exit(1);
    }
    *capacity = new_capacity;
    return new_tokens;
}

// Free tokens array
void free_tokens(Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        free(tokens[i].value);
    }
    free(tokens);
}

// Handle unterminated string literals
void handle_unterminated_string(int line, int column, Token* tokens, int count) {
    fprintf(stderr, "Error: Unterminated string literal at line %d, column %d\n", line, column);
    free_tokens(tokens, count);
    exit(1);
}

// Handle unterminated comments
void handle_unterminated_comment(int line, int column, Token* tokens, int count) {
    fprintf(stderr, "Error: Unterminated multi-line comment at line %d, column %d\n", line, column);
    free_tokens(tokens, count);
    exit(1);
}

// Handle unknown characters
void handle_unknown_character(char character, int line, int column) {
    fprintf(stderr, "Warning: Unknown character '%c' at line %d, column %d. Skipping.\n", character, line, column);
}

// Tokenize identifiers and keywords
void tokenize_identifier(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };
    int j = 0, start_column = *column;
    while (isalnum(code[*i]) || code[*i] == '_') {
        buffer[j++] = code[(*i)++];
        (*column)++;
    }
    buffer[j] = '\0';

    tokens[(*count)++] = (Token){
        is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER,
        _strdup(buffer),
        line,
        start_column
    };
}

// Tokenize operators
void tokenize_operator(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[3] = { 0 };
    int start_column = *column;
    buffer[0] = code[*i];

    if ((strchr("=<>!+-", code[*i]) && code[*i + 1] == '=') ||
        ((code[*i] == '&' || code[*i] == '|') && code[*i + 1] == code[*i])) {
        buffer[1] = code[++(*i)];
        (*column)++;
    }
    buffer[2] = '\0';

    tokens[(*count)++] = (Token){ TOKEN_OPERATOR, _strdup(buffer), line, start_column };
    (*i)++;
    (*column)++;
}

// Tokenize literals
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
        while (isdigit(code[*i]) || (code[*i] == '.' && !has_decimal)) {
            if (code[*i] == '.') has_decimal = 1;
            buffer[j++] = code[(*i)++];
            (*column)++;
        }
    }

    buffer[j] = '\0';
    tokens[(*count)++] = (Token){ TOKEN_LITERAL, _strdup(buffer), line, start_column };
}

// Tokenize string literals
void tokenize_string(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };
    int j = 0, start_column = *column;
    (*i)++;
    (*column)++;

    while (code[*i] != '"' && code[*i] != '\0') {
        if (code[*i] == '\\') {
            buffer[j++] = code[(*i)++];
        }
        buffer[j++] = code[(*i)++];
        (*column)++;
    }

    if (code[*i] == '"') {
        (*i)++;
        (*column)++;
    }
    else {
        handle_unterminated_string(line, start_column, tokens, *count);
    }

    buffer[j] = '\0';
    tokens[(*count)++] = (Token){ TOKEN_STRING, _strdup(buffer), line, start_column };
}

// Tokenize symbols
void tokenize_symbol(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[2] = { code[*i], '\0' };
    tokens[(*count)++] = (Token){ TOKEN_SYMBOL, _strdup(buffer), line, *column };
    (*i)++;
    (*column)++;
}

// Tokenize comments
void tokenize_comment(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    if (code[*i + 1] == '/') {
        while (code[*i] != '\n' && code[*i] != '\0') {
            (*i)++;
            (*column)++;
        }
    }
    else if (code[*i + 1] == '*') {
        (*i) += 2;
        (*column) += 2;
        while (!(code[*i] == '*' && code[*i + 1] == '/') && code[*i] != '\0') {
            if (code[*i] == '\n') {
                line++;
                *column = 1;
            }
            else {
                (*column)++;
            }
            (*i)++;
        }

        if (code[*i] == '*' && code[*i + 1] == '/') {
            (*i) += 2;
            (*column) += 2;
        }
        else {
            handle_unterminated_comment(line, *column, tokens, *count);
        }
    }
}

// Tokenize the input
Token* tokenize(const char* code, int* token_count) {
    int capacity = 100;
    Token* tokens = malloc(capacity * sizeof(Token));
    if (!tokens) {
        fprintf(stderr, "Error: Initial memory allocation failed\n");
        exit(1);
    }

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
            i++;
            continue;
        }

        if (count >= capacity) {
            tokens = resize_tokens(tokens, &capacity);
        }

        if (isalpha(code[i]) || code[i] == '_') {
            tokenize_identifier(code, &i, &column, line, tokens, &count);
        }
        else if (strchr("=+*-<>!&|", code[i])) {
            tokenize_operator(code, &i, &column, line, tokens, &count);
        }
        else if (isdigit(code[i]) || (code[i] == '0' && (code[i + 1] == 'x' || code[i + 1] == 'b'))) {
            tokenize_literal(code, &i, &column, line, tokens, &count);
        }
        else if (code[i] == '"') {
            tokenize_string(code, &i, &column, line, tokens, &count);
        }
        else if (strchr(";(){}", code[i])) {
            tokenize_symbol(code, &i, &column, line, tokens, &count);
        }
        else if (code[i] == '/' && (code[i + 1] == '/' || code[i + 1] == '*')) {
            tokenize_comment(code, &i, &column, line, tokens, &count);
        }
        else {
            handle_unknown_character(code[i], line, column);
            i++;
            column++;
        }
    }

    tokens[count++] = (Token){ TOKEN_EOF, _strdup(""), line, column };
    *token_count = count;
    return tokens;
}
