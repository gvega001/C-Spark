// lexer.c
#include "lexer.h"
#include "utils.h"
#include "debugger.h"
#include "error_reporting.h"
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <limits.h>
#include <stdio.h>

#define COLOR_RED "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_RESET "\033[0m"

// List of keywords
// --- Updated Keywords Array ---
const char* keywords[] = {
    "let", "print", "if", "else", "for", "func", "return",
    "struct", "record", "interface", "virtual", "try", "catch", "defer",
    "switch", "case", "default", "break", "enum"
};

// Global variables for user-defined keywords
static const char** user_defined_keywords = NULL;
static int user_defined_keywords_count = 0;


// Compute the Levenshtein distance between two strings
int levenshtein_distance(const char* s1, const char* s2) {
    int len1 = strlen(s1), len2 = strlen(s2);
    int** dp = malloc((len1 + 1) * sizeof(int*));
    for (int i = 0; i <= len1; i++) {
        dp[i] = malloc((len2 + 1) * sizeof(int));
    }
    for (int i = 0; i <= len1; i++) dp[i][0] = i;
    for (int j = 0; j <= len2; j++) dp[0][j] = j;
    for (int i = 1; i <= len1; i++) {
        for (int j = 1; j <= len2; j++) {
            if (s1[i - 1] == s2[j - 1])
                dp[i][j] = dp[i - 1][j - 1];
            else {
                int min = dp[i - 1][j - 1];
                if (dp[i - 1][j] < min) min = dp[i - 1][j];
                if (dp[i][j - 1] < min) min = dp[i][j - 1];
                dp[i][j] = 1 + min;
            }
        }
    }
    int result = dp[len1][len2];
    for (int i = 0; i <= len1; i++) free(dp[i]);
    free(dp);
    return result;
}

// Function to set user-defined keywords
void set_user_defined_keywords(const char** keywords, int count) {
    user_defined_keywords = keywords;
    user_defined_keywords_count = count;
}

// Collect an error
void collect_error(Error* errors, int* error_count, int line, int column, const char* message) {
    errors[*error_count].line = line;
    errors[*error_count].column = column;
    errors[*error_count].message = utils_safe_strdup(message);
    (*error_count)++;
}

// Check if a string is a keyword
int is_keyword(const char* str) {
    // Check built-in keywords
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return 1;
        }
    }
    // Check user-defined keywords
    for (int i = 0; i < user_defined_keywords_count; i++) {
        if (strcmp(str, user_defined_keywords[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// --- Token Array Resizing & Freeing ---
Token* resize_tokens(Token* tokens, int* capacity) {
    int new_capacity = *capacity * 2;
    Token* new_tokens = realloc(tokens, new_capacity * sizeof(Token));
    if (!new_tokens) {
        report_error("Lexer", 0, 0, "Memory allocation failed during token resizing");
        free_tokens(tokens, *capacity);
        exit(1);
    }
    *capacity = new_capacity;
    return new_tokens;
}

// Free tokens array
void free_tokens(Token* tokens, int count) {
    if (!tokens) return; // Prevent double-free errors

    for (int i = 0; i < count; i++) {
        if (tokens[i].value) {
            free(tokens[i].value);
        }
    }
    free(tokens);
}

// Handle unterminated string literals
void handle_unterminated_string(int line, int column, const char* code, int position, Token* tokens, int count, int start) {
    char message[256];
    snprintf(message, sizeof(message),
        "Unterminated string literal at line %d, column %d. Hint: Ensure the string is closed with a matching quote.",
        line, column);
    report_error("Lexer", line, column, message);
    free_tokens(tokens, count);
    exit(1);
}

// Handle unterminated comments
void handle_unterminated_comment(int line, int column, Token* tokens, int count) {
    char message[256];
    snprintf(message, sizeof(message),
        "Unterminated multi-line comment at line %d, column %d. Multi-line comments must end with '*/'.",
        line, column);
    report_error("Lexer", line, column, message);
    free_tokens(tokens, count);
    exit(1);
}

// Handle unknown characters
void handle_unknown_character(char character, int line, int column) {
    char tmp[2] = { character, '\0' };
    report_warning(line, column, "Unknown character encountered");

    const char* closest_keyword = NULL;
    int best_score = INT_MAX;

    // Check built-in keywords
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        int score = levenshtein_distance(tmp, keywords[i]);
        if (score < best_score) {
            best_score = score;
            closest_keyword = keywords[i];
        }
    }
    // Check user-defined keywords
    for (int i = 0; i < user_defined_keywords_count; i++) {
        int score = levenshtein_distance(tmp, user_defined_keywords[i]);
        if (score < best_score) {
            best_score = score;
            closest_keyword = user_defined_keywords[i];
        }
    }
    if (closest_keyword && best_score <= 2) {
        char suggestion[256];
        snprintf(suggestion, sizeof(suggestion), "Did you mean '%s'?", closest_keyword);
        report_warning(line, column, suggestion);
    }
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
        utils_safe_strdup(buffer),
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

    tokens[(*count)++] = (Token){ TOKEN_OPERATOR, utils_safe_strdup(buffer), line, start_column };
    (*i)++;
    (*column)++;
}

// Process hexadecimal or binary literals
void process_hex_or_binary_literal(
    const char* code,
    int* i,
    int* column,
    char* buffer,
    int* j
) {
    buffer[(*j)++] = code[(*i)++];
    buffer[(*j)++] = code[(*i)++];
    (*column) += 2;

    while (isxdigit(code[*i]) ||
        (buffer[1] == 'b' && (code[*i] == '0' || code[*i] == '1'))) {
        buffer[(*j)++] = code[(*i)++];
        (*column)++;
    }
}

// Process decimal literals
void process_decimal_literal(
    const char* code,
    int* i,
    int* column,
    char* buffer,
    int* j
) {
    int has_decimal = 0;
    while (isdigit(code[*i]) || (code[*i] == '.' && !has_decimal)) {
        if (code[*i] == '.') has_decimal = 1;
        buffer[(*j)++] = code[(*i)++];
        (*column)++;
    }
}

// Tokenize literals
void tokenize_literal(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };
    int j = 0, start_column = *column;

    if (code[*i] == '0' && (code[*i + 1] == 'x' || code[*i + 1] == 'b')) {
        // Process hexadecimal or binary literal
        process_hex_or_binary_literal(code, i, column, buffer, &j);
    }
    else {
        // Process decimal literal
        process_decimal_literal(code, i, column, buffer, &j);
    }

    buffer[j] = '\0'; // Null-terminate the string
    tokens[(*count)++] = (Token){ TOKEN_LITERAL, utils_safe_strdup(buffer), line, start_column };
}

// Process string content
void process_string_content(
    const char* code,
    int* i,
    int* column,
    char* buffer,
    int* j,
    int line,
    int start_column,
    int start_position,
    Token* tokens,
    int* count
) {
    while (code[*i] != '"' && code[*i] != '\0') {
        if (code[*i] == '\\') {  // Handle escape sequences
            buffer[(*j)++] = code[(*i)++];
        }
        else if (code[*i] == '$' && code[*i + 1] == '{') {  // Start of interpolation
            buffer[(*j)++] = code[(*i)++];
            buffer[(*j)++] = code[(*i)++];
            while (code[*i] != '}' && code[*i] != '\0') {  // Read until closing brace
                buffer[(*j)++] = code[(*i)++];
            }
            if (code[*i] == '}') {  // Include closing brace
                buffer[(*j)++] = code[(*i)++];
            }
            else {  // Unterminated interpolation
                handle_unterminated_string(line, start_column, code, start_position, tokens, *count, NULL);
            }
        }
        else {
            buffer[(*j)++] = code[(*i)++];
        }
        (*column)++;
    }
}

// Tokenize string literals
void tokenize_string(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[256] = { 0 };  // Buffer to hold the string value
    int j = 0, start_column = *column;
    int start_position = *i;

    (*i)++; // Skip the opening quote
    (*column)++;

    // Process the string content
    process_string_content(code, i, column, buffer, &j, line, start_column, start_position, tokens, count);

    if (code[*i] == '"') {  // Closing quote
        (*i)++;
        (*column)++;
    }
    else {  // Handle unterminated string
        handle_unterminated_string(line, start_column, code, start_position, tokens, *count, NULL);
    }

    buffer[j] = '\0';  // Null-terminate the string
    tokens[(*count)++] = (Token){ TOKEN_STRING,utils_safe_strdup(buffer), line, start_column };
}

// Tokenize symbols
void tokenize_symbol(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    char buffer[2] = { code[*i], '\0' };

    // Handle semicolon as an optional symbol
    if (code[*i] == ';') {
        tokens[(*count)++] = (Token){ TOKEN_SYMBOL, utils_safe_strdup(buffer), line, *column };
        (*i)++;  // Move past semicolon
        (*column)++;
        return;  // Stop further processing
    }

    // Handle colon for switch-case and type
    if (code[*i] == ':') {
        tokens[(*count)++] = (Token){ TOKEN_COLON, utils_safe_strdup(buffer), line, *column };
        (*i)++;
        (*column)++;
        return;
    }

    // Default handling for other symbols
    tokens[(*count)++] = (Token){ TOKEN_SYMBOL, utils_safe_strdup(buffer), line, *column };
    (*i)++;
    (*column)++;
}


void process_single_line_comment(const char* code, int* i, int* column) {
    while (code[*i] != '\n' && code[*i] != '\0') {
        (*i)++;
        (*column)++;
    }
}

void process_multi_line_comment(
    const char* code,
    int* i,
    int* column,
    int* line,
    Token* tokens,
    int* count
) {
    (*i) += 2;  // Skip "/*"
    (*column) += 2;

    while (!(code[*i] == '*' && code[*i + 1] == '/') && code[*i] != '\0') {
        if (code[*i] == '\n') {
            (*line)++;
            *column = 1;
        }
        else {
            (*column)++;
        }
        (*i)++;
    }

    if (code[*i] == '*' && code[*i + 1] == '/') {
        (*i) += 2;  // Skip closing "*/"
        (*column) += 2;
    }
    else {
        handle_unterminated_comment(*line, *column, tokens, *count);
    }
}

// Tokenize comments
void tokenize_comment(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    if (code[*i + 1] == '/') {
        // Process single-line comment
        process_single_line_comment(code, i, column);
    }
    else if (code[*i + 1] == '*') {
        // Process multi-line comment
        process_multi_line_comment(code, i, column, &line, tokens, count);
    }
}

// Handle unknown characters and advance
void handle_unknown_character_and_advance(const char* code, int* i, int* column, int line) {
    handle_unknown_character(code[*i], line, *column);
    (*i)++;
    (*column)++;
}

// Handle whitespace
void handle_whitespace(const char* code, int* i, int* line, int* column) {
    if (code[*i] == '\n') {
        (*line)++;
        *column = 1;
    }
    else {
        (*column)++;
    }
    (*i)++;
}

int dispatch_tokenizer(
    const char* code,
    int* i,
    int* column,
    int line,
    Token* tokens,
    int* count
) {
    if (isalpha(code[*i]) || code[*i] == '_') {
        tokenize_identifier(code, i, column, line, tokens, count);
    }
    else if (strchr("=+*-<>!&|", code[*i])) {
        tokenize_operator(code, i, column, line, tokens, count);
    }
    else if (isdigit(code[*i]) || (code[*i] == '0' && (code[*i + 1] == 'x' || code[*i + 1] == 'b'))) {
        tokenize_literal(code, i, column, line, tokens, count);
    }
    else if (code[*i] == '"') {
        tokenize_string(code, i, column, line, tokens, count);
    }
    else if (strchr(";(){}", code[*i])) {
        tokenize_symbol(code, i, column, line, tokens, count);
    }
    else if (code[*i] == '/' && (code[*i + 1] == '/' || code[*i + 1] == '*')) {
        tokenize_comment(code, i, column, line, tokens, count);
    }
    else if (strchr(",;(){}", code[*i])) {
        tokenize_symbol(code, i, column, line, tokens, count);
    }
    else {
        return 0; // Unknown character
    }
    return 1; // Token recognized
}

// Add EOF token
void add_eof_token(Token* tokens, int* count, int line, int column) {
    tokens[(*count)++] = (Token){ TOKEN_EOF, utils_safe_strdup(""), line, column };
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
        // Handle whitespace
        if (isspace(code[i])) {
            handle_whitespace(code, &i, &line, &column);
            continue;
        }

        // Resize tokens if needed
        if (count >= capacity) {
            tokens = resize_tokens(tokens, &capacity);
        }

        // Dispatch tokenizer
        if (!dispatch_tokenizer(code, &i, &column, line, tokens, &count)) {
            handle_unknown_character_and_advance(code, &i, &column, line);
        }
    }

    // Add EOF token
    add_eof_token(tokens, &count, line, column);

    *token_count = count;
    return tokens;
}

// Summarize errors
void summarize_errors(int error_count, int warning_count) {
    if (error_count > 0) {
        fprintf(stderr, COLOR_RED "Tokenization completed with %d error(s).\n" COLOR_RESET, error_count);
    }
    if (warning_count > 0) {
        fprintf(stderr, COLOR_YELLOW "%d warning(s) encountered during tokenization.\n" COLOR_RESET, warning_count);
    }
    if (error_count == 0 && warning_count == 0) {
        fprintf(stdout, "Tokenization completed successfully with no issues.\n");
    }
}
bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}
void tokenize_for_loop(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    if (strncmp(&code[*i], "let", 3) == 0 && is_whitespace(code[*i + 3])) {
        tokens[(*count)++] = (Token){ TOKEN_KEYWORD, utils_safe_strdup("let"), line, *column };
        *i += 3;
        *column += 3;
        return;
    }
}

void tokenize_record(const char* code, int* i, int* column, int line, Token* tokens, int* count) {
    if (strncmp(&code[*i], "record", 6) == 0 && is_whitespace(code[*i + 6])) {
        tokens[(*count)++] = (Token){ TOKEN_KEYWORD, utils_safe_strdup("record"), line, *column };
        *i += 6;
        *column += 6;
        return;
    }
}

