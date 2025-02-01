#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdbool.h>

void debug_print(const char* message);
void breakpoint(int line);
void step_debug(const char* statement, int line);
void inspect_variable(const char* var_name, int value);

#endif // DEBUGGER_H
