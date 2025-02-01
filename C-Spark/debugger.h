// debugger.h
#ifndef DEBUGGER_H
#define DEBUGGER_H

#include <stdbool.h>

void debug_print(const char* message);
void breakpoint(int line);
void step_debug(const char* statement, int line);

#endif // DEBUGGER_H
