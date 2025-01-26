#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "transpile.h"
#include "utils.h"
#define _CRT_SECURE_NO_WARNINGS

// Safe memory allocation safe_strdup helper
void* validate_input(const void* input, const char* error_message, int should_exit) {
    // Check if the input is NULL
    if (!input) {
        // Print the error message to stderr
        fprintf(stderr, "Error: %s\n", error_message);
        // Exit if the input is critical
        if (should_exit) {
            exit(EXIT_FAILURE); // Exit if the input is critical
        }
        return NULL; // Return NULL if not exiting
    }
    return (void*)input; // Cast input back to void* for flexibility
}

// Safe memory allocation for duplication of strings
static char* safe_strdup(const char* str) {
    // Validate the input string
    validate_input(str, "NULL string passed to safe_strdup", 0); // Validate the string input
    
    // Calculate the length of the string
    size_t len = strlen(str) + 1;  // Include space for the null terminator
    char* copy = malloc(len);      // Allocate memory
    copy = validate_input(copy, "Memory allocation failed for strdup", 1); // Validate the allocated memory

    strcpy_s(copy, len, str);      // Copy the string safely
    return copy;
}

// Utility to append strings dynamically
static char* append_code(char* dest, const char* src) {
    // Calculate the new size needed for the concatenated string, including the null terminator
    size_t new_size = strlen(dest) + strlen(src) + 1;

    // Allocate or reallocate memory
    char* result = realloc(dest, new_size);
    result = validate_input(result, "Memory allocation failed in append_code", 1); // Validate the pointer

    // Safely concatenate the strings
    strcat_s(result, new_size, src);

    return result;
}

// Add indentation to the generated code
static void add_indentation(char** code, int level) {
    for (int i = 0; i < level; i++) {
        *code = append_code(*code, "    ");
    }
}

// Create a new IR node
static IRNode* create_ir_node(const char* code, int line, int column, const char* original_code, Scope* scope) {
    IRNode* ir = safe_malloc(sizeof(IRNode)); // Allocate memory for IRNode
    if (!ir) {
        fprintf(stderr, "Error: Memory allocation failed for IRNode\n");
        exit(EXIT_FAILURE);
    }

    // Safely duplicate the code
    if (code) {
        ir->code = utils_safe_strdup(code);
    }
    else {
        ir->code = NULL;
    }
    ir->line = line;
    ir->column = column;
    ir->original_code = original_code ? safe_strdup(original_code) : NULL;
    ir->scope = scope ? scope : NULL;  // Use NULL if scope is not provided
    ir->next = NULL;
    return ir;
}

// Append an IR node to the list
void append_ir_node(IRNode** head, IRNode* new_node) {
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

// Free memory allocated for IR nodes
static void free_ir_list(IRNode* head) {
    while (head) {
        IRNode* temp = head;
        head = head->next;
        free(temp->code);
        free(temp->original_code);
        free(temp);
    }
}

// Generate a unique name for overloaded functions
static char* generate_overloaded_name(const char* base_name, ASTNode* parameters) {
    char* name = safe_malloc(strlen(base_name) + 32);
    snprintf(name, strlen(base_name) + 32, "%s_%dparams", base_name, parameters->child_count);
    return name;
}

// Transpile a function node
static void transpile_function(ASTNode* node, IRNode** ir_list) {
    char* overloaded_name = generate_overloaded_name(node->token.value, node->children[0]);
    char code[256];

    snprintf(code, sizeof(code), "void %s(", overloaded_name);
    for (int i = 0; i < node->children[0]->child_count; i++) {
        char param_code[64];
        ASTNode* param = node->children[0]->children[i];

        if (param->child_count > 0) {
            snprintf(param_code, sizeof(param_code), "%s = %s", param->token.value, param->children[0]->token.value);
        }
        else {
            snprintf(param_code, sizeof(param_code), "%s", param->token.value);
        }
        strcat_s(code, sizeof(code), param_code);
        if (i < node->children[0]->child_count - 1) strcat_s(code, sizeof(code), ", ");
    }
    strcat_s(code, sizeof(code), ") {");

    IRNode* func_node = create_ir_node(code, node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, func_node);

    transpile_to_ir(node->children[1], ir_list);

    IRNode* end_node = create_ir_node("}", node->token.line, node->token.column, node->token.value,NULL);
    append_ir_node(ir_list, end_node);

    free(overloaded_name);
}

// Transpile a string interpolation node
void transpile_string_interpolation(ASTNode* node, IRNode** ir_list) {
    size_t buffer_size = 256; // Initial buffer size
    char* code = malloc(buffer_size); // Allocate memory for the code buffer
    if (!code) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return;
    }
    strcpy_s(code, buffer_size, "printf(\""); // Start with printf format string

    const char* str = node->token.value;
    for (int i = 0; str[i] != '\0'; ++i) {
        if (str[i] == '$' && str[i + 1] == '{') {
            // Ensure enough space in the buffer
            if (strlen(code) + 3 >= buffer_size) {
                buffer_size *= 2;
                code = realloc(code, buffer_size);
                if (!code) {
                    fprintf(stderr, "Error: Memory reallocation failed\n");
                    return;
                }
            }

            strcat_s(code, buffer_size, "%s"); // Add placeholder for embedded expression
            i += 2;

            char expr[128] = { 0 };
            int j = 0;
            while (str[i] != '}' && str[i] != '\0') {
                expr[j++] = str[i++];
            }
            if (str[i] == '}') i++; // Skip '}'

            IRNode* expr_node = create_ir_node(expr, node->token.line, node->token.column, NULL,NULL);
            append_ir_node(ir_list, expr_node); // Append embedded expression
        }
        else {
            // Ensure enough space in the buffer
            if (strlen(code) + 2 >= buffer_size) {
                buffer_size *= 2;
                code = realloc(code, buffer_size);
                if (!code) {
                    fprintf(stderr, "Error: Memory reallocation failed\n");
                    return;
                }
            }

            char temp[2] = { str[i], '\0' };
            strcat_s(code, buffer_size, temp); // Append regular characters
        }
    }

    // Add closing format string and complete printf statement
    if (strlen(code) + 5 >= buffer_size) {
        buffer_size += 5;
        code = realloc(code, buffer_size);
        if (!code) {
            fprintf(stderr, "Error: Memory reallocation failed\n");
            return;
        }
    }
    strcat_s(code, buffer_size, "\\n\");");

    IRNode* ir_node = create_ir_node(code, node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, ir_node); // Add to the IR list
    free(code);
}

// Transpile a struct node
static void transpile_struct(ASTNode* node, IRNode** ir_list) {
    char code[256];
    snprintf(code, sizeof(code), "struct %s {", node->token.value);
    IRNode* struct_node = create_ir_node(code, node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, struct_node);

    for (int i = 0; i < node->child_count; i++) {
        char field_code[128];
        snprintf(field_code, sizeof(field_code), "%s %s;", node->children[i]->token.type, node->children[i]->token.value);
        IRNode* field_node = create_ir_node(field_code, node->children[i]->token.line, node->children[i]->token.column, node->children[i]->token.value, NULL);
        append_ir_node(ir_list, field_node);
    }

    IRNode* end_node = create_ir_node("};", node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, end_node);
}
// Generate code from IR
char* generate_code_from_ir(IRNode* ir_list, const char* lang) {
    // Start with an empty string
    char* code = safe_malloc(1);
    code[0] = '\0';

    // Iterate through the IR linked list
    IRNode* current = ir_list;
    while (current) {
        code = append_code(code, current->code);
        code = append_code(code, "\n"); // Add a newline for readability
        current = current->next;
    }

    // Handle any language-specific requirements (e.g., boilerplate)
    if (strcmp(lang, "c") == 0) {
        char* boilerplate = "#include <stdio.h>\n\n";
        char* final_code = append_code(safe_strdup(boilerplate), code);
        free(code);
        return final_code;
    }

    return code;
}

// Transpile the AST into target code
char* transpile(ASTNode* tree) {
    // Initialize the IR list
    IRNode* ir_list = NULL;

    // Generate IR from the AST
    transpile_to_ir(tree, &ir_list);

    // Convert IR to code
    char* code = generate_code_from_ir(ir_list, "c");

    // Free the IR list
    free_ir_list(ir_list);

    return code;
}

// Handle unsupported node types
static void handle_unsupported_node(ASTNode* node) {
    fprintf(stderr, "Warning: Unsupported node type %d at line %d, column %d. Skipping.\n",
        node->type, node->token.line, node->token.column);
}
static void transpile_to_ir(ASTNode* node, IRNode** ir_list) {
    transpile_to_ir_with_scope(node, ir_list, NULL); // Call the overloaded version with NULL scope
}

// transpile_block function
void transpile_block(ASTNode* block_node, IRNode** ir_list, Scope* current_scope) {
    if (!block_node || block_node->type != NODE_BLOCK) {
        fprintf(stderr, "Error: Invalid block node (type=%d, expected=%d)\n",
            block_node ? block_node->type : -1, NODE_BLOCK);
        return;
    }

    // Log the beginning of block processing
    printf("Transpiling block at line %d, column %d\n", block_node->token.line, block_node->token.column);

    // Create a new scope for the block
    Scope* block_scope = create_scope("block_scope", current_scope);
    if (!block_scope) {
        fprintf(stderr, "Error: Failed to create scope for block at line %d, column %d\n",
            block_node->token.line, block_node->token.column);
        return;
    }

    // Add comments to the IR to indicate a new block
    char comment[256];
    snprintf(comment, sizeof(comment), "// Start of block (line %d, column %d)",
        block_node->token.line, block_node->token.column);
    IRNode* comment_node = create_ir_node(comment, block_node->token.line, block_node->token.column, NULL, current_scope);
    append_ir_node(ir_list, comment_node);

    // Handle empty blocks explicitly
    if (block_node->child_count == 0) {
        printf("Warning: Empty block at line %d, column %d\n",
            block_node->token.line, block_node->token.column);
    }

    // Process each child in the block
    for (int i = 0; i < block_node->child_count; i++) {
        ASTNode* child = block_node->children[i];
        if (!child) {
            fprintf(stderr, "Warning: Null child in block at index %d (line %d)\n",
                i, block_node->token.line);
            continue;
        }

        printf("Processing child %d of type %d\n", i, child->type);
        transpile_to_ir_with_scope(child, ir_list, block_scope); // Recursively process children
    }

    // Add a comment to indicate the end of the block
    snprintf(comment, sizeof(comment), "// End of block (line %d, column %d)",
        block_node->token.line, block_node->token.column);
    comment_node = create_ir_node(comment, block_node->token.line, block_node->token.column, NULL, current_scope);
    append_ir_node(ir_list, comment_node);

    // Free the block scope
    free_scope(block_scope);

    printf("Finished transpiling block at line %d, column %d\n",
        block_node->token.line, block_node->token.column);
}

// Transpile the AST node into IR recursively
static void transpile_to_ir_with_scope(ASTNode* node, IRNode** ir_list, Scope* current_scope) {
    if (!node) return;

    // If the current_scope is NULL, use a default global or top-level scope
    if (!current_scope) {
        static Scope* global_scope = NULL;
        if (!global_scope) {
            global_scope = create_scope("global", NULL); // Create a global scope if it doesn't exist
        }
        current_scope = global_scope;
    }
    switch (node->type) {
    case NODE_FUNCTION: {
        Scope* function_scope = create_scope("function_scope", current_scope);
        transpile_function(node, ir_list);
        free_scope(function_scope);
        break;
    }
    case NODE_STRING_INTERPOLATION:
        transpile_string_interpolation(node, ir_list);
        break;
    case NODE_STRUCT: {
        Scope* struct_scope = create_scope("struct_scope", current_scope);
        transpile_struct(node, ir_list);
        free_scope(struct_scope);
        break;
    }
    case NODE_BLOCK:
        transpile_block(node, ir_list, NULL);
        break;
    default:
        fprintf(stderr, "Warning: Unsupported node type %d at line %d, column %d\n",
            node->type, node->token.line, node->token.column);
        handle_unsupported_node(node);
        break;
    }
    // Recursively process children
    for (int i = 0; i < node->child_count; i++) {
        transpile_to_ir_with_scope(node->children[i], ir_list, current_scope);
    }
}

//This function converts record AST nodes into C struct definitions.
void transpile_record(ASTNode* node, IRNode** ir_list) {
    if (node->type != NODE_STRUCT) return;

    char buffer[512];
    snprintf(buffer, sizeof(buffer), "typedef struct %s {", node->token.value);

    // Create the struct definition
    IRNode* record_node = create_ir_node(buffer, node->token.line, node->token.column, NULL, NULL);
    append_ir_node(ir_list, record_node);

    // Add fields to the struct
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* field = node->children[i];
        snprintf(buffer, sizeof(buffer), "    int %s;", field->token.value); // Default to int
        IRNode* field_node = create_ir_node(buffer, field->token.line, field->token.column, NULL, NULL);
        append_ir_node(ir_list, field_node);
    }
    // End the struct definition
    snprintf(buffer, sizeof(buffer), "} %s;", node->token.value);
    IRNode* end_record_node = create_ir_node(buffer, node->token.line, node->token.column, NULL, NULL);
    append_ir_node(ir_list, end_record_node);
}

// Create a new scope
static Scope* create_scope(const char* name, Scope* parent) {
    Scope* new_scope = malloc(sizeof(Scope));
    if (!new_scope) {
        fprintf(stderr, "Error: Memory allocation failed for Scope\n");
        exit(EXIT_FAILURE);
    }
    new_scope->name = name ? _strdup(name) : NULL;
    new_scope->parent = parent;
    return new_scope;
}
// Free memory allocated for a scope
void free_scope(Scope* scope) {
    if (!scope) return;
    free(scope->name);
    free(scope);
}
