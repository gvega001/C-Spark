#ifndef TRANSPILE_H
#define TRANSPILE_H

#include "parser.h"

// Intermediate Representation (IR) Node structure
typedef struct IRNode {
    char* code;           // Generated code for this IR node
    int line;             // Line number in the source code
    int column;           // Column number in the source code
    char* original_code;  // Original source for mapping
    struct IRNode* next;  // Pointer to the next IR node
    char* scope;          // Scope identifier for nested scopes
    char* function_name;  // Name for handling overloaded functions
} IRNode;

// Safe string duplication function
char* safe_strdup(const char* str);

// Append strings dynamically
char* append_code(char* dest, const char* src);

// Transpile the AST into target code
char* transpile(ASTNode* tree);

// Error handler for unsupported nodes
void handle_unsupported_node(ASTNode* node);

// Add indentation for code readability
void add_indentation(char** code, int level);

// Create a new IR node
IRNode* create_ir_node(const char* code, int line, int column, const char* original_code);

// Append an IR node to the IR list
void append_ir_node(IRNode** head, IRNode* new_node);

// Free the IR list
void free_ir_list(IRNode* head);

// Generate a unique name for overloaded functions
char* generate_overloaded_name(const char* base_name, ASTNode* parameters);

// Transpile AST nodes for different constructs
void transpile_function(ASTNode* node, IRNode** ir_list);
void transpile_string_interpolation(ASTNode* node, IRNode** ir_list);
void transpile_struct(ASTNode* node, IRNode** ir_list);
void transpile_to_ir(ASTNode* node, IRNode** ir_list);

// Generate code from IR
char* generate_code_from_ir(IRNode* ir_list, const char* lang);
void transpile_record(ASTNode* node, IRNode** ir_list);
#endif // TRANSPILE_H
