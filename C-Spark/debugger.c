// debugger.c
#include <stdio.h>
#include "debugger.h"

#ifdef DEBUG_MODE
static bool debugging_enabled = true;
#else
static bool debugging_enabled = false;
#endif

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
}

// A function to simulate stepping through code (for example, one statement at a time)
void step_debug(const char* statement, int line) {
    if (debugging_enabled) {
        fprintf(stdout, "[DEBUG] Executing line %d -> %s\n", line, statement);
        fprintf(stdout, "[DEBUG] Press Enter to step...");
        while (getchar() != '\n');
    }
}
