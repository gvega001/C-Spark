/* =========================
 * Enhanced Type Inference
 * ========================= */

 // src/type_inference.c
#include "type_inference.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


static InferredType* create_inferred_type(Type* type) {
    if (!type) {
        fprintf(stderr, "[type_inference] Invalid inferred type.\n");
        return NULL;
    }

    InferredType* inferredType = (InferredType*)malloc(sizeof(InferredType));
    if (!inferredType) {
        fprintf(stderr, "[type_inference] Memory allocation failed for inferred type.\n");
        exit(EXIT_FAILURE);
    }
    inferredType->deducedType = type;
    return inferredType;
}
//
//void infer_type(const char* expression) {
//    if (!expression) {
//        fprintf(stderr, "[type_inference] Invalid expression for type inference.\n");
//        return;
//    }
//
//    // Simulated type inference (should be replaced with actual expression parsing and analysis)
//    Type* deducedType = NULL;
//    if (strchr(expression, '.') != NULL) deducedType = TYPE_FLOAT;
//    else if (strspn(expression, "0123456789") == strlen(expression)) deducedType = TYPE_INT;
//    else if (strcmp(expression, "true") == 0 || strcmp(expression, "false") == 0) deducedType = TYPE_BOOL;
//    else if (expression[0] == '"' && expression[strlen(expression) - 1] == '"') deducedType = TYPE_STRING;
//    else {
//        fprintf(stderr, "[type_inference] Unable to deduce type for expression: %s\n", expression);
//        return;
//    }
//
//    InferredType* inferred = create_inferred_type(deducedType);
//    if (inferred) {
//        printf("[type_inference] Expression '%s' inferred as type '%s'.\n", expression, deducedType->name);
//    }
//    free(inferred);
//}
