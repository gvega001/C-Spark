#include <stdio.h>
#include "debugger.h"
#include "inline_hints.h"  // Include the Inline Hints system

void enable_debugging() {
    debugging_enabled = 1;
}

void disable_debugging() {
    debugging_enabled = 0;
}
// Print a debug message if debug mode is enabled
void debug_print(const char* message) {
    if (debugging_enabled) {
        fprintf(stdout, "[DEBUG] %s\n", message);
    }
}


// A simple breakpoint: halts execution until user presses Enter
void breakpoint(int line) {
    if (!debugging_enabled) return;

    fprintf(stdout, "[DEBUG] Breakpoint reached at line %d. Press Enter to continue...", line);
    while (getchar() != '\n');

    // Inline Hint: Warn if the breakpoint is inside a loop, indicating a possible infinite loop
    provide_hint(HINT_POSSIBLE_INFINITE_LOOP, line, 0);
}

// A function to simulate stepping through code (for example, one statement at a time)
void step_debug(const char* statement, int line) {
    if (debugging_enabled) {
        fprintf(stdout, "[DEBUG] Executing line %d -> %s\n", line, statement);
        fprintf(stdout, "[DEBUG] Press Enter to step...");
        while (getchar() != '\n');
    }
}

// Inspect a variable’s value during debugging
void inspect_variable(const char* var_name, int value) {
    if (debugging_enabled) {
        fprintf(stdout, "[DEBUG] Variable '%s' = %d\n", var_name, value);

        // Inline Hint: Check if the variable is uninitialized
        if (value == 0) {
            provide_hint(HINT_UNINITIALIZED_VARIABLE, __LINE__, 0);
            suggest_fix(HINT_UNINITIALIZED_VARIABLE, __LINE__, 0);
        }
    }
}
