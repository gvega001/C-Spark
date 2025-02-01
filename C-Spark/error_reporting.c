#include "error_reporting.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

#define COLOR_RED "\033[1;31m"
#define COLOR_YELLOW "\033[1;33m"
#define COLOR_RESET "\033[0m"

// Global error storage
Error error_log[MAX_ERRORS];
int error_count = 0;

// Backward Compatible Error Reporting
#ifdef LEGACY_MODE
void report_error(int line, int column, const char* message) {
    fprintf(stderr, COLOR_RED "[Error] Line %d, Column %d: %s\n" COLOR_RESET, line, column, message);
    log_error(line, column, message);
}
#else
void report_error(const char* stage, int line, int column, const char* message) {
    fprintf(stderr, COLOR_RED "[%s Error] Line %d, Column %d: %s\n" COLOR_RESET, stage, line, column, message);
    log_error(line, column, message);
}
#endif

// Log warnings
void report_warning(int line, int column, const char* message) {
    fprintf(stderr, COLOR_YELLOW "  Warning (Line %d, Column %d): %s\n" COLOR_RESET, line, column, message);
}

// Backward Compatible Error Logging
#ifdef LEGACY_MODE
void log_error(int line, int column, const char* message) {
    if (error_count < MAX_ERRORS) {
        error_log[error_count].line = line;
        error_log[error_count].column = column;
        error_log[error_count].message = strdup(message);
        error_count++;
    }
    else {
        fprintf(stderr, "Error log is full. Unable to track more errors.\n");
    }
}
#else
void log_error(int line, int column, const char* message) {
    if (error_count < MAX_ERRORS) {
        error_log[error_count].line = line;
        error_log[error_count].column = column;
        error_log[error_count].message = utils_safe_strdup(message);  // Uses safe memory allocation
        error_count++;
    }
    else {
        fprintf(stderr, "Error log is full. Unable to track more errors.\n");
    }
}
#endif

// Print all errors
void print_error_log() {
    for (int i = 0; i < error_count; i++) {
        fprintf(stderr, COLOR_RED "Error %d: Line %d, Column %d: %s\n" COLOR_RESET,
            i + 1, error_log[i].line, error_log[i].column, error_log[i].message);
    }
}

// Free allocated error memory
void free_error_log() {
    for (int i = 0; i < error_count; i++) {
        free(error_log[i].message);
    }
    error_count = 0;
}
