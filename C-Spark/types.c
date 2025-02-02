// src/types.c
#include "types.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define the Type structure. You can extend it with more attributes later.
typedef struct Type {
    char* name;
    size_t size;
    // Other type-related attributes (e.g., flags, alignment) can be added here.
} Type;

// Helper function to create a new type.
static Type* create_type(const char* name, size_t size) {
    Type* t = (Type*)malloc(sizeof(Type));
    if (!t) {
        fprintf_s(stderr, "[types] Memory allocation failed for type '%s'.\n", name);
        exit(EXIT_FAILURE);
    }
    t->name = utils_safe_strdup(name);
    t->size = size;
    return t;
}

// Global pointers for basic types (or store them in your compiler’s symbol table).
Type* TYPE_INT = NULL;
Type* TYPE_FLOAT = NULL;
Type* TYPE_BOOL = NULL;
Type* TYPE_STRING = NULL;
Type* TYPE_CHAR = NULL;
Type* TYPE_VOID = NULL;
Type* TYPE_NULL = NULL;
Type* TYPE_ERROR = NULL;
Type* TYPE_UNKNOWN = NULL;
Type* TYPE_ANY = NULL;
Type* TYPE_ARRAY = NULL;
Type* TYPE_FUNCTION = NULL;
Type* TYPE_STRUCT = NULL;
Type* TYPE_ENUM = NULL;


void define_basic_types() {
    TYPE_INT = create_type("int", sizeof(int));
    TYPE_FLOAT = create_type("float", sizeof(float));
    TYPE_BOOL = create_type("bool", sizeof(int)); // using int to represent bool
    TYPE_STRING = create_type("string", sizeof(char*)); // string as pointer to char
    TYPE_CHAR = create_type("char", sizeof(char));
    TYPE_VOID = create_type("void", 0);
    TYPE_NULL = create_type("null", 0);
    TYPE_ERROR = create_type("error", 0);
    TYPE_UNKNOWN = create_type("unknown", 0);
    TYPE_ANY = create_type("any", 0);
    TYPE_ARRAY = create_type("array", sizeof(void*)); // array as pointer to any type
    TYPE_FUNCTION = create_type("function", sizeof(void*)); // function as pointer to any type
    TYPE_STRUCT = create_type("struct", sizeof(void*)); // struct as pointer to any type
    TYPE_ENUM = create_type("enum", sizeof(int)); // enum as int
    

    printf("[types] Basic types defined.\n");
}
