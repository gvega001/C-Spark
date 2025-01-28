#pragma once
#ifndef ERROR_REPORTING_H
#define ERROR_REPORTING_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ERRORS 100

typedef struct {
    int line;
    int column;
    char* message;
} Error;

extern Error error_log[MAX_ERRORS];
extern int error_count;

void report_error(const char* stage, int line, int column, const char* message);
void log_error(int line, int column, const char* message);
void print_error_log();
void free_error_log();

#endif // ERROR_REPORTING_H
