// include/operators.h
#ifndef OPERATORS_H
#define OPERATORS_H


// Function pointer type for operator overloads
typedef void (*OperatorFunction)(void*, void*);

// Function declarations
void define_operator_overloads();
void register_operator_overload(const char* symbol, void (*func)(void*, void*));
OperatorFunction get_operator_function(const char* symbol);

#endif // OPERATORS_H
