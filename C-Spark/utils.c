#include "utils.h"
#include <string.h>

// Safe malloc implementation
void* safe_malloc(size_t size) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Error: Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Safe realloc implementation
void* safe_realloc(void* ptr, size_t size) {
    ptr = realloc(ptr, size);
    if (!ptr) {
        fprintf(stderr, "Error: Memory reallocation failed.\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

// Safe strdup implementation
char* utils_safe_strdup(const char* str) {
    if (!str) {
        fprintf(stderr, "Error: Attempted to duplicate a NULL string.\n");
        return NULL;
    }
    size_t len = strlen(str) + 1;
    char* copy = safe_malloc(len);
    strncpy_s(copy, len, str, len - 1);
    copy[len - 1] = '\0'; // Ensure null-termination
    return copy;
}

// Centralized error logging
void log_error(const char* message, int line, int column) {
    fprintf(stderr, "Error: %s at line %d, column %d\n", message, line, column);
}

// Append strings dynamically
char* utils_append_code(char* dest, const char* src) {
    size_t new_size = strlen(dest) + strlen(src) + 1;
    dest = safe_realloc(dest, new_size);
    strcat_s(dest, new_size, src);
    return dest;
}