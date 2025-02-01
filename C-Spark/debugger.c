// File: debugger.c (Enhanced with AST Visualization & Memory Tracking)

#include <stdio.h>
#include "debugger.h"
#include "inline_hints.h"  // Include the Inline Hints system
#include "parser.h"   // For AST traversal
#include "transpile.h" // For execution tracking


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
void inspect_variable(const char* var_name, int value, void* address) {
    if (debugging_enabled) {
        fprintf(stdout, "[DEBUG] Variable '%s' = %d (Address: %p)\n", var_name, value, address);

        // Inline Hint: Check if the variable is uninitialized
        if (value == 0) {
            provide_hint(HINT_UNINITIALIZED_VARIABLE, __LINE__, 0);
            suggest_fix(HINT_UNINITIALIZED_VARIABLE, __LINE__, 0);
        }
    }
}

// AST Visualization (Print AST Nodes Recursively)
void visualize_ast(ASTNode* node, int depth) {
    if (!node) return;

    // Indentation for tree-like structure
    for (int i = 0; i < depth; i++) printf("  ");

    // Print the node's type and associated token
    printf("[%d] %s\n", node->type, node->token.value);  // Assuming token has a 'value' field

    // Recursively print children
    for (int i = 0; i < node->child_count; i++) {
        visualize_ast(node->children[i], depth + 1);
    }
}


// Hook into Transpiler for Execution Tracking
void track_execution(IRNode* ir) {
    if (debugging_enabled) {
        printf("[EXECUTION] Executing IR Node of type: %d\n", ir->type);
        // Assuming IRNode has a 'type' field instead of 'instruction'
    }
}
