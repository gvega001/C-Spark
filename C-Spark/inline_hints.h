#ifndef INLINE_HINTS_H
#define INLINE_HINTS_H

#include <stdio.h>

#define COLOR_GREEN "\033[1;32m"
#define COLOR_RESET "\033[0m"

// Hint categories
typedef enum {
    HINT_MISSING_SEMICOLON,
    HINT_UNINITIALIZED_VARIABLE,
    HINT_FUNCTION_ARG_MISMATCH,
    HINT_TYPE_MISMATCH,
    HINT_POSSIBLE_INFINITE_LOOP,
    HINT_READABILITY_ISSUE,
    HINT_SECURITY_WARNING
} HintType;

// Structure for storing hints
typedef struct {
    HintType type;
    int line;
    int column;
    const char* message;
    const char* fix_suggestion;
} Hint;

// Function declarations
void provide_hint(HintType type, int line, int column);
void suggest_fix(HintType type, int line, int column);

#endif // INLINE_HINTS_H
