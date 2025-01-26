#ifndef TRANSPILE_H
#define TRANSPILE_H

#include "parser.h"

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
void transpile_record(ASTNode* node, IRNode** ir_list);
void transpile_block(ASTNode* block_node, IRNode** ir_list, Scope* current_scope);
// Original function (without Scope*)
static void transpile_to_ir(ASTNode* node, IRNode** ir_list);

// Overloaded function (with Scope*)
static void transpile_to_ir_with_scope(ASTNode* node, IRNode** ir_list, Scope* current_scope);

// Generate code from IR
char* generate_code_from_ir(IRNode* ir_list, const char* lang);
void transpile_record(ASTNode* node, IRNode** ir_list);
Scope* create_scope(const char* name, Scope* parent);
void free_scope(Scope* scope);
void* validate_input(const void* input, const char* error_message, int should_exit);
static void initialize_ir_node(IRNode* ir, const char* code, int line, int column, const char* original_code, Scope* scope);
#endif // TRANSPILE_H
