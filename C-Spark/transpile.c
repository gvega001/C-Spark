#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "transpile.h"
#include "utils.h"
#include "achievements.h"
#define _CRT_SECURE_NO_WARNINGS

void transpile_to_ir_with_scope(ASTNode* node, IRNode** ir_list, Scope* current_scope);
static void transpile_to_ir(ASTNode* node, IRNode** ir_list);

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
    char* copy = safe_malloc(len);      // Allocate memory
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
    // Append spaces based on the indentation level
    for (int i = 0; i < level; i++) {
        *code = append_code(*code, "    ");
    }
}
// Intermediate Representation (IR) Node structure
static void initialize_ir_node(IRNode* ir, const char* code, int line, int column, const char* original_code, Scope* scope) {
    // Safely duplicate the code
    ir->code = code ? safe_strdup(code) : NULL;// Use NULL if code is not provided
    ir->line = line;
    ir->column = column;
    ir->original_code = original_code ? safe_strdup(original_code) : NULL;
    ir->scope = scope ? scope : NULL;  // Use NULL if scope is not provided
    ir->next = NULL;
}

// Create a new IR node
static IRNode* create_ir_node(const char* code, int line, int column, const char* original_code, Scope* scope) {
    // Allocate memory for IRNode and validate it
    IRNode* ir = validate_input(safe_malloc(sizeof(IRNode)), "Memory allocation failed for IRNode", 1);

    // Initialize the IRNode fields
    initialize_ir_node(ir, code, line, column, original_code, scope);

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

        if (temp->code) {
            free(temp->code);
        }
        if (temp->original_code) {
            free(temp->original_code);
        }
        free(temp);
    }
}

// Generate a unique name for overloaded functions
static char* generate_overloaded_name(const char* base_name, ASTNode* parameters) {
    // Allocate memory for the name
    char* name = validate_input(safe_malloc(strlen(base_name) + 32), "Memory allocation failed in generate_overloaded_name", 1);
    snprintf(name, strlen(base_name) + 32, "%s_%dparams", base_name, parameters->child_count);
    return name;
}
// Append the parameters to the function signature
static void append_function_parameters(char* code, size_t code_size, ASTNode* parameters) {
    for (int i = 0; i < parameters->child_count; i++) {
        char param_code[64];
        ASTNode* param = parameters->children[i];

        if (param->child_count > 0) {
            snprintf(param_code, sizeof(param_code), "%s = %s", param->token.value, param->children[0]->token.value);
        }
        else {
            snprintf(param_code, sizeof(param_code), "%s", param->token.value);
        }

        strcat_s(code, code_size, param_code);
        if (i < parameters->child_count - 1) {
            strcat_s(code, code_size, ", ");
        }
    }
}

IRNode* create_ir_node_optimized(const char* code, int line, int column, const char* original_code) {
    static char buffer[256]; // Shared buffer
    snprintf(buffer, sizeof(buffer), "%s", code);
    IRNode* ir = malloc(sizeof(IRNode));
    if (!ir) { exit(1); }
    ir->code = utils_safe_strdup(buffer);
    ir->line = line;
    ir->column = column;
    ir->original_code = original_code ? utils_safe_strdup(original_code) : NULL;
    ir->next = NULL;
    return ir;
}

IRNode* allocate_ir_nodes(size_t batch_size) {
    return safe_malloc(batch_size * sizeof(IRNode));
}

// Transpile a function node
void transpile_function(ASTNode* node, IRNode** ir_list) {
    // Check if the node is a function
    char* overloaded_name = generate_overloaded_name(node->token.value, node->children[0]);
    char code[256];

    // Generate the function signature
    snprintf(code, sizeof(code), "void %s(", overloaded_name);

    // Append the parameters to the function signature
    append_function_parameters(code, sizeof(code), node->children[0]);

    strcat_s(code, sizeof(code), ") {");

    IRNode* func_node = create_ir_node(code, node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, func_node);

    transpile_to_ir(node->children[1], ir_list);

    IRNode* end_node = create_ir_node("}", node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, end_node);

    free(overloaded_name);
}

static char* ensure_buffer_space(char* buffer, size_t* buffer_size, size_t additional_space) {
    if (strlen(buffer) + additional_space >= *buffer_size) {
        *buffer_size *= 2;
        buffer = safe_realloc(buffer, *buffer_size);
        buffer = validate_input(buffer, "Memory reallocation failed in transpile_string_interpolation", 1);
    }
    return buffer;
}
static void process_embedded_expression(
    const char* str,
    int* i,
    char* code,
    size_t* buffer_size,
    IRNode** ir_list,
    int line,
    int column
) {
    strcat_s(code, *buffer_size, "%s"); // Add placeholder for the embedded expression
    *i += 2; // Skip the "${"

    char expr[128] = { 0 };
    int j = 0;

    // Extract the embedded expression
    while (str[*i] != '}' && str[*i] != '\0') {
        expr[j++] = str[(*i)++];
    }
    if (str[*i] == '}') (*i)++; // Skip the closing "}"

    // Create an IR node for the embedded expression
    IRNode* expr_node = create_ir_node(expr, line, column, NULL, NULL);
    append_ir_node(ir_list, expr_node);
}
static void process_regular_character(const char* str, int i, char* code, size_t* buffer_size) {
    char temp[2] = { str[i], '\0' };
    strcat_s(code, *buffer_size, temp); // Append the character to the buffer
}

// Transpile a string interpolation node
void transpile_string_interpolation(ASTNode* node, IRNode** ir_list) {
    size_t fmt_size = 256;
    char* fmt_str = safe_malloc(fmt_size);
    fmt_str[0] = '\0';
    strcat_s(fmt_str, fmt_size, "printf(\"");

    // Prepare a separate string to collect the embedded expressions (arguments)
    size_t args_size = 256;
    char* args_str = safe_malloc(args_size);
    args_str[0] = '\0';
    int arg_count = 0;

    const char* input = node->token.value;
    for (int i = 0; input[i] != '\0'; i++) {
        if (input[i] == '$' && input[i + 1] == '{') {
            // Skip the "${"
            i += 2;

            // Append a placeholder into the format string
            fmt_str = ensure_buffer_space(fmt_str, &fmt_size, 3);
            strcat_s(fmt_str, fmt_size, "%s");

            // Extract the embedded expression (up to the closing '}')
            char expr_buffer[128] = { 0 };
            int j = 0;
            while (input[i] != '}' && input[i] != '\0' && j < (int)(sizeof(expr_buffer) - 1)) {
                expr_buffer[j++] = input[i++];
            }
            expr_buffer[j] = '\0';
            // (Assume a closing '}' is found; you might add error handling if not.)

            // Append the expression to the arguments list
            if (arg_count > 0) {
                args_str = ensure_buffer_space(args_str, &args_size, 3);
                strcat_s(args_str, args_size, ", ");
            }
            args_str = ensure_buffer_space(args_str, &args_size, strlen(expr_buffer) + 1);
            strcat_s(args_str, args_size, expr_buffer);
            arg_count++;
        }
        else {
            // Append a regular character into the format string.
            // If the character is a double-quote or a backslash, escape it.
            if (input[i] == '"' || input[i] == '\\') {
                fmt_str = ensure_buffer_space(fmt_str, &fmt_size, 2);
                strcat_s(fmt_str, fmt_size, "\\");
            }
            char temp[2] = { input[i], '\0' };
            fmt_str = ensure_buffer_space(fmt_str, &fmt_size, 2);
            strcat_s(fmt_str, fmt_size, temp);
        }
    }
    // Close the format string and add a newline.
    fmt_str = ensure_buffer_space(fmt_str, &fmt_size, 6);
    strcat_s(fmt_str, fmt_size, "\\n\"");

    // If there are embedded expressions, add a comma and then the arguments.
    if (arg_count > 0) {
        fmt_str = ensure_buffer_space(fmt_str, &fmt_size, args_size + 3);
        strcat_s(fmt_str, fmt_size, ", ");
        strcat_s(fmt_str, fmt_size, args_str);
    }
    fmt_str = ensure_buffer_space(fmt_str, &fmt_size, 3);
    strcat_s(fmt_str, fmt_size, ");");

    IRNode* ir_node = create_ir_node(fmt_str, node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, ir_node);

    free(fmt_str);
    free(args_str);
}

static void add_struct_fields(ASTNode* node, IRNode** ir_list) {
    for (int i = 0; i < node->child_count; i++) {
        char field_code[128];
        snprintf(field_code, sizeof(field_code), "%s %s;", node->children[i]->token.type, node->children[i]->token.value);

        IRNode* field_node = create_ir_node(
            field_code,
            node->children[i]->token.line,
            node->children[i]->token.column,
            node->children[i]->token.value,
            NULL
        );

        append_ir_node(ir_list, field_node);
    }
}

// Transpile a struct node
static void transpile_struct(ASTNode* node, IRNode** ir_list) {
    char code[256];
    snprintf(code, sizeof(code), "struct %s {", node->token.value);

    IRNode* struct_node = create_ir_node(code, node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, struct_node);

    // Add fields to the struct
    add_struct_fields(node, ir_list);

    IRNode* end_node = create_ir_node("};", node->token.line, node->token.column, node->token.value, NULL);
    append_ir_node(ir_list, end_node);
}
static char* initialize_code_buffer() {
    char* code = validate_input(safe_malloc(1), "Memory allocation failed for code buffer", 1);
    code[0] = '\0';
    return code;
}

static char* process_ir_list(IRNode* ir_list) {
    char* code = initialize_code_buffer();
    IRNode* current = ir_list;

    while (current) {
        code = append_code(code, current->code);
        code = append_code(code, "\n"); // Add a newline for readability
        current = current->next;
    }

    return code;
}
static char* apply_language_boilerplate(const char* lang, char* code) {
    if (strcmp(lang, "c") == 0) {
        char* boilerplate = "#include <stdio.h>\n\n";
        char* final_code = append_code(safe_strdup(boilerplate), code);
        free(code);
        return final_code;
    }

    return code; // No boilerplate needed for other languages
}

// Generate code from IR
char* generate_code_from_ir(IRNode* ir_list, const char* lang) {
    // Process the IR linked list into code
    char* code = process_ir_list(ir_list);

    // Handle language-specific boilerplate
    code = apply_language_boilerplate(lang, code);

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

static void process_block_children(ASTNode* block_node, IRNode** ir_list, Scope* block_scope) {
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
}
static void add_block_comments(ASTNode* block_node, IRNode** ir_list, const char* comment_prefix, Scope* scope) {
    char comment[256];
    snprintf(comment, sizeof(comment), "%s block (line %d, column %d)",
        comment_prefix, block_node->token.line, block_node->token.column);
    IRNode* comment_node = create_ir_node(comment, block_node->token.line, block_node->token.column, NULL, scope);
    append_ir_node(ir_list, comment_node);
}

// transpile_block function
void transpile_block(ASTNode* block_node, IRNode** ir_list, Scope* current_scope) {
    if (!block_node || block_node->type != NODE_BLOCK) {
        fprintf(stderr, "Error: Invalid block node (type=%d, expected=%d)\n",
            block_node ? block_node->type : -1, NODE_BLOCK);
        return;
    }

    printf("Transpiling block at line %d, column %d\n", block_node->token.line, block_node->token.column);

    Scope* block_scope = create_scope("block_scope", current_scope);
    if (!block_scope) {
        fprintf(stderr, "Error: Failed to create scope for block at line %d, column %d\n",
            block_node->token.line, block_node->token.column);
        return;
    }

    // Add start comment
    add_block_comments(block_node, ir_list, "// Start of", block_scope);

    // Handle empty block
    if (block_node->child_count == 0) {
        printf("Warning: Empty block at line %d, column %d\n",
            block_node->token.line, block_node->token.column);
    }

    // Process children
    process_block_children(block_node, ir_list, block_scope);

    // Add end comment
    add_block_comments(block_node, ir_list, "// End of", block_scope);

    free_scope(block_scope);

    printf("Finished transpiling block at line %d, column %d\n",
        block_node->token.line, block_node->token.column);
}
static void process_ast_node_with_scope(ASTNode* node, IRNode** ir_list, Scope* current_scope) {
    Achievement achievements[ACH_MILESTONES_COUNT];
    initialize_achievements(achievements);
    switch (node->type) {
    case NODE_FUNCTION: {
        Scope* function_scope = create_scope("function_scope", current_scope);
        transpile_function(node, ir_list);
        if (!achievements[ACH_FIRST_FUNCTION].unlocked) {
            unlock_achievement(achievements, ACH_FIRST_FUNCTION);
        }
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
        transpile_block(node, ir_list, current_scope);
        break;
    default:
        fprintf(stderr, "Warning: Unsupported node type %d at line %d, column %d\n",
            node->type, node->token.line, node->token.column);
        handle_unsupported_node(node);
        break;
    }
}


static void transpile_to_ir(ASTNode* node, IRNode** ir_list) {
    transpile_to_ir_with_scope(node, ir_list, NULL); // Call the overloaded version with NULL scope
}

// Transpile the AST node into IR recursively
static void transpile_to_ir_with_scope(ASTNode* node, IRNode** ir_list, Scope* current_scope) {
    if (!node) return;

    if (!current_scope) {
        static Scope* global_scope = NULL;
        if (!global_scope) {
            global_scope = create_scope("global", NULL); // Create global scope
        }
        current_scope = global_scope;
    }

    // Process the current node using the new function
    process_ast_node_with_scope(node, ir_list, current_scope);

    // Recursively process children
    for (int i = 0; i < node->child_count; i++) {
        transpile_to_ir_with_scope(node->children[i], ir_list, current_scope);
    }

}
static void add_record_fields(ASTNode* node, IRNode** ir_list) {
    char buffer[512];
    for (int i = 0; i < node->child_count; i++) {
        ASTNode* field = node->children[i];
        snprintf(buffer, sizeof(buffer), "    int %s;", field->token.value); // Default to int
        IRNode* field_node = create_ir_node(buffer, field->token.line, field->token.column, NULL, NULL);
        append_ir_node(ir_list, field_node);
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
    add_record_fields(node, ir_list);

    // End the struct definition
    snprintf(buffer, sizeof(buffer), "} %s;", node->token.value);
    IRNode* end_record_node = create_ir_node(buffer, node->token.line, node->token.column, NULL, NULL);
    append_ir_node(ir_list, end_record_node);
}


// Create a new scope
static Scope* create_scope(const char* name, Scope* parent) {
    // Allocate memory for the scope
    Scope* new_scope = validate_input(safe_malloc(sizeof(Scope)), "Memory allocation failed for Scope", 1);
    // Initialize the scope fields
    new_scope->name = name ? _strdup(name) : NULL;
    new_scope->parent = parent;// Set the parent scope
    return new_scope;
}
// Free memory allocated for a scope
void free_scope(Scope* scope) {
    if (!scope) return;
    free(scope->name);
    free(scope);
}
