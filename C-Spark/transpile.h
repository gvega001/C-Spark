#ifndef TRANSPILE_H
#define TRANSPILE_H

#include "parser.h"

// Scope structure
typedef struct Scope {
    char* name;                // Scope name
    struct Scope* parent;      // Parent scope for nested hierarchies
} Scope;

// Intermediate Representation (IR) Node structure
typedef struct IRNode {
    char* code;           // Generated code for this IR node
    int line;             // Line number in the source code
    int column;           // Column number in the source code
    char* original_code;  // Original source for mapping
    Scope* scope;         // Pointer to scope
    struct IRNode* next;  // Pointer to the next IR node
    void* metadata; // Additional data for future use
} IRNode;

// Public API functions
char* safe_strdup(const char* str);                      // Safe string duplication
char* append_code(char* dest, const char* src);          // Append strings dynamically
char* transpile(ASTNode* tree);                          // Transpile the AST into target code
void handle_unsupported_node(ASTNode* node);             // Error handler for unsupported nodes
void add_indentation(char** code, int level);            // Add indentation for code readability
IRNode* create_ir_node(const char* code, int line, int column, const char* original_code); // Create a new IR node
void append_ir_node(IRNode** head, IRNode* new_node);    // Append an IR node to the IR list
void free_ir_list(IRNode* head);                         // Free the IR list
char* generate_overloaded_name(const char* base_name, ASTNode* parameters); // Generate a unique name for overloaded functions
void transpile_function(ASTNode* node, IRNode** ir_list);// Transpile a function node
void transpile_string_interpolation(ASTNode* node, IRNode** ir_list); // Transpile string interpolation
void transpile_struct(ASTNode* node, IRNode** ir_list);  // Transpile a struct node
void transpile_record(ASTNode* node, IRNode** ir_list);  // Transpile a record node
void transpile_block(ASTNode* block_node, IRNode** ir_list, Scope* current_scope); // Transpile a block node
Scope* create_scope(const char* name, Scope* parent);    // Create a new scope
void free_scope(Scope* scope);                           // Free a scope
char* generate_code_from_ir(IRNode* ir_list, const char* lang); // Generate code from IR

#endif // TRANSPILE_H
