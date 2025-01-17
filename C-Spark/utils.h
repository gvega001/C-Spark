#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>

// Safe memory allocation
void* safe_malloc(size_t size);
void* safe_realloc(void* ptr, size_t size);

// Safe string duplication
char* utils_safe_strdup(const char* str);

// Centralized error logging
void log_error(const char* message, int line, int column);

// String appending with dynamic memory
char* utils_append_code(char* dest, const char* src);

#endif // UTILS_H
