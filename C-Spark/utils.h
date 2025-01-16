#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>

// Safe malloc wrapper
void* safe_malloc(size_t size);

// Safe realloc wrapper
void* safe_realloc(void* ptr, size_t size);

#endif // UTILS_H
