// src/operators.c
#include "operators.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define a structure for storing operator overloads
typedef struct OperatorOverload {
    char* symbol;
    void (*function)(void*, void*); // Function pointer for operation
} OperatorOverload;

// Define a static table to store operator overloads
#define MAX_OPERATORS 20
static OperatorOverload operator_table[MAX_OPERATORS];
static int operator_count = 0;

// POSIX strdup compatibility for MSVC
#ifdef _MSC_VER
#define strdup _strdup
#endif

// Function to register an operator overload
void register_operator_overload(const char* symbol, void (*func)(void*, void*)) {
    if (operator_count >= MAX_OPERATORS) {
        fprintf(stderr, "[operators] Operator overload table is full.\n");
        return;
    }
    operator_table[operator_count].symbol = strdup(symbol);
    if (!operator_table[operator_count].symbol) {
        fprintf(stderr, "[operators] Memory allocation failed for operator symbol.\n");
        exit(EXIT_FAILURE);
    }
    operator_table[operator_count].function = func;
    operator_count++;
}

// Function to find an operator overload
void (*get_operator_function(const char* symbol))(void*, void*) {
    for (int i = 0; i < operator_count; i++) {
        if (strcmp(operator_table[i].symbol, symbol) == 0) {
            return operator_table[i].function;
        }
    }
    return NULL; // No overload found
}

// Default operator overloads
void int_add(void* a, void* b) {
    int result = *(int*)a + *(int*)b;
    printf("[operators] Overloaded '+' result: %d\n", result);
}

void int_subtract(void* a, void* b) {
    int result = *(int*)a - *(int*)b;
    printf("[operators] Overloaded '-' result: %d\n", result);
}

// Function to initialize operator overloading system
void define_operator_overloads() {
    printf("[operators] Defining operator overloads.\n");

    // Registering default overloads
    register_operator_overload("+", int_add);
    register_operator_overload("-", int_subtract);

    printf("[operators] Default overloads registered.\n");
}

// Cleanup function to free allocated memory
void cleanup_operator_overloads() {
    for (int i = 0; i < operator_count; i++) {
        free(operator_table[i].symbol);
    }
    operator_count = 0;
    printf("[operators] Operator overloads cleaned up.\n");
}
