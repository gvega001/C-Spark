// include/types.h
#ifndef TYPES_H
#define TYPES_H

#include <stddef.h>

typedef struct Type Type;

extern Type* TYPE_INT;
extern Type* TYPE_FLOAT;
extern Type* TYPE_BOOL;
extern Type* TYPE_STRING;
extern Type* TYPE_CHAR;
extern Type* TYPE_VOID;
extern Type* TYPE_NULL;
extern Type* TYPE_ERROR;
extern Type* TYPE_UNKNOWN;
extern Type* TYPE_ANY;
extern Type* TYPE_ARRAY;
extern Type* TYPE_FUNCTION;
extern Type* TYPE_STRUCT;
extern Type* TYPE_ENUM;
    

void define_basic_types();
Type* infer_type(const char* variable, void* value);
#endif // TYPES_H

