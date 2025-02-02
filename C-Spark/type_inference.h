// include/type_inference.h
#ifndef TYPE_INFERENCE_H
#define TYPE_INFERENCE_H

#include "types.h"

typedef struct InferredType {
    Type* deducedType;
    char* name;
} InferredType;

//void infer_type(const char* expression);
static InferredType* create_inferred_type(Type* type);

#endif // TYPE_INFERENCE_H

