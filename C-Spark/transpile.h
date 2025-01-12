#ifndef TRANSPILE_H
#define TRANSPILE_H

#include "parser.h"

// Public Function Declarations
// Transpiles an Abstract Syntax Tree (AST) into target code in the specified language
char* transpile_ast(ASTNode* root, const char* lang);

#endif // TRANSPILE_H
#pragma once
