#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdbool.h>
// Declare debugging_enabled globally
static int debugging_enabled = 0; // 0 = Off, 1 = On
void debug_print(const char* message);
void breakpoint(int line);
void step_debug(const char* statement, int line);
void inspect_variable(const char* var_name, int value);
void enable_debugging();
void disable_debugging();
#endif // DEBUGGER_H
