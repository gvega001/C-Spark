/* =========================
 * Enhanced Arrays Handling
 * ========================= */

 // src/arrays.c
#include "arrays.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static ArrayType* create_array_type(Type* baseType, int dimensions) {
    if (!baseType || dimensions <= 0) {
        fprintf(stderr, "[arrays] Invalid array declaration.\n");
        return NULL;
    }

    ArrayType* arrayType = (ArrayType*)malloc(sizeof(ArrayType));
    if (!arrayType) {
        fprintf(stderr, "[arrays] Memory allocation failed for array type.\n");
        exit(EXIT_FAILURE);
    }
    arrayType->baseType = baseType;
    arrayType->dimensions = dimensions;
    return arrayType;
}

void handle_array_declaration(const char* baseTypeName, int dimensions) {
    // Validate base type
    if (!baseTypeName || dimensions <= 0) {
        fprintf(stderr, "[arrays] Invalid array declaration for '%s'.\n", baseTypeName);
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
        fprintf(stderr, "[arrays] Unknown base type: %s\n", baseTypeName);
        return;
    }

    ArrayType* arrayType = create_array_type(baseType, dimensions);
    if (arrayType) {
        printf("[arrays] Defined an array of type '%s' with %d dimension(s).\n", baseTypeName, dimensions);
    }
    free(arrayType);
}
