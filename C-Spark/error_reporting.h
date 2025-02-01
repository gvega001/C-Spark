#ifndef ERROR_REPORTING_H
#define ERROR_REPORTING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ERRORS 100

// Error struct
typedef struct {
    int line;
    int column;
    char* message;
} Error;

// Global error storage
extern Error error_log[MAX_ERRORS];
extern int error_count;

// Legacy Mode (for backward compatibility)
#ifdef LEGACY_MODE
void report_error(int line, int column, const char* message);
void log_error(int line, int column, const char* message);
#else
    // Newer implementation with "stage" tracking
void report_error(const char* stage, int line, int column, const char* message);
void log_error(int line, int column, const char* message);
#endif

void report_warning(int line, int column, const char* message);
void print_error_log();
void free_error_log();

#endif // ERROR_REPORTING_H
