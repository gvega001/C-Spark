// include/arrays.h
#ifndef ARRAYS_H
#define ARRAYS_H
#include "types.h"

typedef struct ArrayType {
    Type* baseType;
    int dimensions;
} ArrayType;

void handle_array_declaration(const char* baseType, int dimensions);

#endif // ARRAYS_H

