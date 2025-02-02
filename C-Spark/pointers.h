// include/pointers.h
#ifndef POINTERS_H
#define POINTERS_H
#include "types.h"

typedef struct PointerType {
    Type* baseType;
    int isReference; // 1 for reference, 0 for pointer
} PointerType;

void process_pointer_reference(const char* baseType, int isReference);

#endif // POINTERS_H

