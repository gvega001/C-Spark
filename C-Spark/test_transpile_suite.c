#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "transpile.h"
#include "parser.h"
#include "lexer.h"
#include "test_transpile_suite.h"


// Utility function for running individual test cases
void run_test(const char* description, int (*test_function)()) {
    printf("Running test: %s\n", description);
    int result = test_function();
    if (result) {
        printf("Test passed: %s\n\n", description);
    }
    else {
        printf("Test failed: %s\n\n", description);
    }
}

// Implementation of safe_strdup
char* safe_strdup(const char* str) {
    if (!str) {
        fprintf(stderr, "Error: Attempted to strdup a NULL pointer.\n");
        return NULL;
    }
    char* duplicate = strdup(str);
    if (!duplicate) {
        fprintf(stderr, "Error: Memory allocation failed in safe_strdup.\n");
        exit(1);
    }
    return duplicate;
}

char* append_code(char* base, const char* addition) {
    if (!base || !addition) {
        fprintf(stderr, "Error: NULL string passed to append_code.\n");
        return NULL;
    }
    size_t base_len = strlen(base);
    size_t addition_len = strlen(addition);
    char* result = realloc(base, base_len + addition_len + 1); // +1 for null terminator
    if (!result) {
        fprintf(stderr, "Error: Memory allocation failed in append_code.\n");
        free(base);
        exit(1);
    }
    strcat_s(result, base_len + addition_len + 1, addition); // Fix: Provide buffer size
    return result;
}

// Implementation of create_ir_node
IRNode* create_ir_node(const char* code, int line, int column, const char* comment) {
    IRNode* node = malloc(sizeof(IRNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed in create_ir_node.\n");
        exit(1);
    }
    node->code = safe_strdup(code);
    node->line = line;
    node->column = column;
    node->next = NULL;
    return node;
}

// Implementation of free_ir_list
void free_ir_list(IRNode* head) {
    while (head) {
        IRNode* temp = head;
        head = head->next;
        free(temp->code);
        free(temp);
    }
}

// Implementation of transpile_to_ir
void transpile_to_ir(ASTNode* root, IRNode** ir_list) {
    if (!root || !ir_list) return;

    if (root->type == NODE_VARIABLE_DECLARATION) {
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "let %s;", root->token.value);
        IRNode* node = create_ir_node(buffer, root->token.line, root->token.column, NULL);
        node->next = *ir_list;
        *ir_list = node;
    }

    for (int i = 0; i < root->child_count; i++) {
        transpile_to_ir(root->children[i], ir_list);
    }
}

// Test safe_strdup function
int test_safe_strdup() {
    const char* original = "Test String";
    char* duplicate = safe_strdup(original);

    int result = (duplicate != NULL && strcmp(original, duplicate) == 0);
    free(duplicate);

    return result;
}

// Test append_code function
int test_append_code() {
    char* base = safe_strdup("Hello");
    base = append_code(base, " World!");

    int result = strcmp(base, "Hello World!") == 0;
    free(base);

    return result;
}

// Test IRNode creation
int test_ir_node_creation() {
    const char* code = "let x = 10;";
    IRNode* node = create_ir_node(code, 1, 5, code);

    int result = (node != NULL &&
        strcmp(node->code, code) == 0 &&
        node->line == 1 &&
        node->column == 5);

    free_ir_list(node);
    return result;
}

// Test transpile_to_ir with a simple AST
int test_transpile_to_ir() {
    Token tokens[] = {
        {TOKEN_KEYWORD, "let", 1, 1},
        {TOKEN_IDENTIFIER, "x", 1, 5},
        {TOKEN_OPERATOR, "=", 1, 7},
        {TOKEN_LITERAL, "10", 1, 9},
        {TOKEN_SYMBOL, ";", 1, 11}
    };

    // Create the root AST node
    ASTNode* root = create_node(NODE_PROGRAM, tokens[0]);
    if (!root) {
        fprintf(stderr, "Error: Failed to allocate memory for root node.\n");
        return 0;
    }

    // Mock child nodes for "let x = 10;"
    ASTNode* declaration = create_node(NODE_VARIABLE_DECLARATION, tokens[1]);
    ASTNode* assignment = create_node(NODE_ASSIGNMENT, tokens[2]);

    // Validate node creation
    if (!declaration || !assignment) {
        fprintf(stderr, "Error: Failed to allocate memory for child nodes.\n");
        free_ast(root);
        return 0;
    }

    add_child(declaration, assignment);
    add_child(root, declaration);

    IRNode* ir_list = NULL;
    transpile_to_ir(root, &ir_list);

    int result = (ir_list != NULL &&
        strcmp(ir_list->code, "let x = 10;") == 0);

    free_ast(root);
    free_ir_list(ir_list);

    return result;
}

// Test error handling for unsupported nodes
int test_unsupported_node_handling() {
    // Unsupported node: struct
    Token struct_token = { TOKEN_KEYWORD, "struct", 1, 1 };
    ASTNode unsupported = { NODE_STRUCT, struct_token, NULL, 0 };
    IRNode* ir_list = NULL;

    transpile_to_ir(&unsupported, &ir_list);

    int result = (ir_list == NULL); // Ensure unsupported node is ignored

    return result;
}
