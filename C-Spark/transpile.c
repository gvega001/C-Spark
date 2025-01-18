#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "transpile.h"
#define _CRT_SECURE_NO_WARNINGS
// Safe strdup implementation
static char* safe_strdup(const char* str) {
    size_t len = strlen(str) + 1;
    char* copy = malloc(len);
    if (!copy) {
        fprintf(stderr, "Error: Memory allocation failed for strdup\n");
        exit(EXIT_FAILURE);
    }
    strncpy_s(copy, len, str, len - 1);
    copy[len - 1] = '\0';
    return copy;
}

// Utility function to append strings dynamically
static char* append_code(char* dest, const char* src) {
    size_t new_size = strlen(dest) + strlen(src) + 1;
    char* result = realloc(dest, new_size);
    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed in append_code\n");
        exit(EXIT_FAILURE);
    }
    strcat_s(result, new_size, src); // Use strcat_s for safer concatenation
    return result;
}

// Error handler for unsupported nodes
static void handle_unsupported_node(ASTNode* node) {
    fprintf(stderr, "Warning: Unsupported node type %d at line %d, column %d. Skipping.\n",
        node->type, node->token.line, node->token.column);
}

// Add indentation for readability
static void add_indentation(char** code, int level) {
    for (int i = 0; i < level; i++) {
        *code = append_code(*code, "    "); // 4 spaces per level
    }
}



// Create a new IR node
static IRNode* create_ir_node(const char* code, int line, int column, const char* original_code) {
    IRNode* ir = safe_malloc(sizeof(IRNode));

    ir->code = safe_strdup(code);
    ir->line = line;
    ir->column = column;
    ir->original_code = original_code ? safe_strdup(original_code) : NULL;
    ir->next = NULL;
    return ir;
}

// Append an IR node to the list
static void append_ir_node(IRNode** head, IRNode* new_node) {
    if (!*head) {
        *head = new_node;
    }
    else {
        IRNode* current = *head;
        while (current->next) {
            current = current->next;
        }
        current->next = new_node;
    }
}

// Free IR list
static void free_ir_list(IRNode* head) {
    while (head) {
        IRNode* temp = head;
        head = head->next;
        free(temp->code);
        free(temp->original_code);
        free(temp);
    }
}

// Function Overloading: Generate unique function names for overloaded functions
static char* generate_overloaded_name(const char* base_name, ASTNode* parameters) {
    char* name = safe_malloc(strlen(base_name) + 32);

    snprintf(name, strlen(base_name) + 32, "%s_%dparams", base_name, parameters->child_count); // Use snprintf for safety
    return name;
}

// Transpile NODE_FUNCTION with Optional Parameters and Overloading
static void transpile_function(ASTNode* node, IRNode** ir_list) {
    char* overloaded_name = generate_overloaded_name(node->token.value, node->children[0]);
    char code[256];
    snprintf(code, sizeof(code), "void %s(", overloaded_name);
    for (int i = 0; i < node->children[0]->child_count; i++) {
        char param_code[64];
        ASTNode* param = node->children[0]->children[i];
        if (param->child_count > 0) { // Default value
            snprintf(param_code, sizeof(param_code), "%s = %s", param->token.value, param->children[0]->token.value);
        }
        else {
            snprintf(param_code, sizeof(param_code), "%s", param->token.value);
        }
        strcat_s(code, sizeof(code), param_code);
        if (i < node->children[0]->child_count - 1) strcat_s(code, sizeof(code), ", ");
    }
    strcat_s(code, sizeof(code), ") {");
    IRNode* func_node = create_ir_node(code, node->token.line, node->token.column, node->token.value);
    append_ir_node(ir_list, func_node);

    transpile_to_ir(node->children[1], ir_list); // Function body

    IRNode* end_node = create_ir_node("}", node->token.line, node->token.column, node->token.value);
    append_ir_node(ir_list, end_node);

    free(overloaded_name);
}

// Transpile NODE_STRING_INTERPOLATION
static void transpile_string_interpolation(ASTNode* node, IRNode** ir_list) {
    char code[512] = "\""; // Start of interpolated string
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* child = node->children[i];
        if (child->type == NODE_LITERAL) {
            strcat_s(code, sizeof(code), child->token.value);
        }
        else if (child->type == NODE_VARIABLE) {
            strcat_s(code, sizeof(code), "{");
            strcat_s(code, sizeof(code), child->token.value);
            strcat_s(code, sizeof(code), "}");
        }
    }
    strcat_s(code, sizeof(code), "\";");
    IRNode* ir_node = create_ir_node(code, node->token.line, node->token.column, node->token.value);
    append_ir_node(ir_list, ir_node);
}

// Transpile NODE_STRUCT
static void transpile_struct(ASTNode* node, IRNode** ir_list) {
    char code[256];
    snprintf(code, sizeof(code), "struct %s {", node->token.value);
    IRNode* struct_node = create_ir_node(code, node->token.line, node->token.column, node->token.value);
    append_ir_node(ir_list, struct_node);

    for (int i = 0; i < node->child_count; i++) {
        char field_code[128];
        snprintf(field_code, sizeof(field_code), "%s %s;", node->children[i]->token.type, node->children[i]->token.value);
        IRNode* field_node = create_ir_node(field_code, node->children[i]->token.line, node->children[i]->token.column,
            node->children[i]->token.value);
        append_ir_node(ir_list, field_node);
    }

    IRNode* end_node = create_ir_node("};", node->token.line, node->token.column, node->token.value);
    append_ir_node(ir_list, end_node);
}

// Recursive function to transpile an AST node into IR
static void transpile_to_ir(ASTNode* node, IRNode** ir_list) {
    if (!node) return;

    switch (node->type) {
    case NODE_FUNCTION:
        transpile_function(node, ir_list);
        break;

    case NODE_STRING_INTERPOLATION:
        transpile_string_interpolation(node, ir_list);
        break;

    case NODE_STRUCT:
        transpile_struct(node, ir_list);
        break;

    default:
        handle_unsupported_node(node);
        break;
    }
}

// Generate code from IR
static char* generate_code_from_ir(IRNode* ir_list, const char* lang) {
    char* code = safe_malloc(1);
    code[0] = '\0';

    IRNode* current = ir_list;
    while (current) {
        code = append_code(code, current->code);
        code = append_code(code, "\n");
        current = current->next;
    }

    return code;
}

// Public function to transpile the AST into target code
char* transpile_ast(ASTNode* root, const char* lang) {
    IRNode* ir_list = NULL;
    transpile_to_ir(root, &ir_list);

    char* code = generate_code_from_ir(ir_list, lang);
    free_ir_list(ir_list);

    return code;
}
