/* =========================
 * Enhanced Pointers Handling
 * ========================= */

 // src/pointers.c
#include "pointers.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static PointerType* create_pointer_type(Type* baseType, int isReference) {
    if (!baseType) {
        fprintf(stderr, "[pointers] Invalid pointer/reference declaration.\n");
        return NULL;
    }

    PointerType* ptrType = (PointerType*)malloc(sizeof(PointerType));
    if (!ptrType) {
        fprintf(stderr, "[pointers] Memory allocation failed for pointer type.\n");
        exit(EXIT_FAILURE);
    }
    ptrType->baseType = baseType;
    ptrType->isReference = isReference;
    return ptrType;
}

void process_pointer_reference(const char* baseTypeName, int isReference) {
    if (!baseTypeName) {
        fprintf(stderr, "[pointers] Invalid base type for pointer/reference.\n");
        return;
    }

    // Simulated type lookup (should be replaced with actual type resolution)
    Type* baseType = NULL;
    if (strcmp(baseTypeName, "int") == 0) baseType = TYPE_INT;
    else if (strcmp(baseTypeName, "float") == 0) baseType = TYPE_FLOAT;
    else if (strcmp(baseTypeName, "bool") == 0) baseType = TYPE_BOOL;
    else if (strcmp(baseTypeName, "char") == 0) baseType = TYPE_CHAR;
    else if (strcmp(baseTypeName, "string") == 0) baseType = TYPE_STRING;
    else {
        fprintf(stderr, "[pointers] Unknown base type: %s\n", baseTypeName);
        return;
    }

    PointerType* ptrType = create_pointer_type(baseType, isReference);
    if (ptrType) {
        printf("[pointers] Defined a %s to type '%s'.\n", isReference ? "reference" : "pointer", baseTypeName);
    }
    free(ptrType);
}
