
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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

// Provide a hint based on detected issues
void provide_hint(HintType type, int line, int column) {
    switch (type) {
    case HINT_MISSING_SEMICOLON:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Missing semicolon. Did you forget to add ';'?\n" COLOR_RESET, line, column);
        break;
    case HINT_UNINITIALIZED_VARIABLE:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Variable is uninitialized. Consider initializing it before use.\n" COLOR_RESET, line, column);
        break;
    case HINT_FUNCTION_ARG_MISMATCH:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Function argument count mismatch. Did you mean to add or remove arguments?\n" COLOR_RESET, line, column);
        break;
    case HINT_TYPE_MISMATCH:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Type mismatch detected. Consider converting the value to the expected type.\n" COLOR_RESET, line, column);
        break;
    case HINT_POSSIBLE_INFINITE_LOOP:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Possible infinite loop detected. Did you mean to add a break condition?\n" COLOR_RESET, line, column);
        break;
    case HINT_READABILITY_ISSUE:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Code readability can be improved. Consider adding proper spacing and comments.\n" COLOR_RESET, line, column);
        break;
    case HINT_SECURITY_WARNING:
        fprintf(stderr, COLOR_GREEN "Hint (Line %d, Column %d): Potential security risk detected. Ensure input validation is in place.\n" COLOR_RESET, line, column);
        break;
    default:
        fprintf(stderr, "Unknown hint detected.\n");
        break;
    }
}

// Suggest an automatic fix
void suggest_fix(HintType type, int line, int column) {
    switch (type) {
    case HINT_MISSING_SEMICOLON:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Add ';' at the end of the statement.\n", line, column);
        break;
    case HINT_UNINITIALIZED_VARIABLE:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Initialize variable with a default value.\n", line, column);
        break;
    case HINT_FUNCTION_ARG_MISMATCH:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Add or remove function arguments to match the definition.\n", line, column);
        break;
    case HINT_TYPE_MISMATCH:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Use type conversion to correct mismatch.\n", line, column);
        break;
    case HINT_POSSIBLE_INFINITE_LOOP:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Add a break condition to the loop.\n", line, column);
        break;
    case HINT_READABILITY_ISSUE:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Format code properly for better readability.\n", line, column);
        break;
    case HINT_SECURITY_WARNING:
        fprintf(stderr, "Suggested Fix (Line %d, Column %d): Add input validation to prevent security risks.\n", line, column);
        break;
    default:
        fprintf(stderr, "Unknown fix suggestion.\n");
        break;
    }
}
