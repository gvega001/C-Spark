// error_reporting.c
#include "error_reporting.h"
#include "utils.h"

// Error log implementation
Error error_log[MAX_ERRORS];
int error_count = 0;

// Centralized error reporting function
void report_error(const char* stage, int line, int column, const char* message) {
    fprintf(stderr, "[%s Error] Line %d, Column %d: %s\n", stage, line, column, message);
    log_error(line, column, message);
}

// Log errors for later review
void log_error(int line, int column, const char* message) {
    if (error_count < MAX_ERRORS) {
        error_log[error_count].line = line;
        error_log[error_count].column = column;
        error_log[error_count].message = utils_safe_strdup(message);
        error_count++;
    }
    else {
        fprintf(stderr, "Error log is full. Unable to track more errors.\n");
    }
}

// Print all logged errors
void print_error_log() {
    for (int i = 0; i < error_count; i++) {
        fprintf(stderr, "Error %d: Line %d, Column %d: %s\n", i + 1, error_log[i].line, error_log[i].column, error_log[i].message);
    }
}

// Free memory allocated for error messages
void free_error_log() {
    for (int i = 0; i < error_count; i++) {
        free(error_log[i].message);
    }
    error_count = 0;
}
