#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
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

    ASTNode* declaration = create_node(NODE_VARIABLE_DECLARATION, tokens[1]);
    if (!declaration) {
        fprintf(stderr, "Error: Failed to allocate memory for declaration node.\n");
        free_ast(root);
        return 0;
    }

    add_child(root, declaration);

    IRNode* ir_list = NULL;
    transpile_to_ir(root, &ir_list);

    int result = (ir_list != NULL &&
        strcmp(ir_list->code, "let x;") == 0); // Validate IR output
    if (!result) {
        fprintf(stderr, "Error: IR generation failed for test_transpile_to_ir.\n");
    }
    else {
        printf("test_transpile_to_ir passed.\n");
    }

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

// Interdependent functions test
void test_interdependent_functions() {
    const char* input = "int a() { return b(); } int b() { return 1; }";
    int token_count = 0;

    // Tokenize input
    Token* tokens = tokenize(input, &token_count);
    if (!tokens) {
        fprintf(stderr, "Error: Tokenization failed for test_interdependent_functions.\n");
        return;
    }

    // Parse tokens into an AST
    ASTNode* tree = parse_program(tokens, token_count);
    if (!tree) {
        fprintf(stderr, "Error: Parsing failed for test_interdependent_functions.\n");
        free_tokens(tokens, token_count);
        return;
    }

    // Transpile the AST
    char* output = transpile(tree);
    if (!output) {
        fprintf(stderr, "Error: Transpilation failed for test_interdependent_functions.\n");
        free_ast(tree);
        free_tokens(tokens, token_count);
        return;
    }

    // Check for function `a` and `b` in the transpiled output
    int result = (strstr(output, "int a()") != NULL && strstr(output, "int b()") != NULL);
    if (!result) {
        fprintf(stderr, "Error: Transpiled output does not contain expected functions.\n");
    }
    else {
        printf("test_interdependent_functions passed.\n");
    }

    // Free allocated memory
    free(output);
    free_ast(tree);
    free_tokens(tokens, token_count);
}


// Borderline syntax test
void test_borderline_syntax() {
    const char* input = "int x = 2147483647;"; // Max int value
    int token_count = 0;

    // Tokenize input
    Token* tokens = tokenize(input, &token_count);
    if (!tokens) {
        fprintf(stderr, "Error: Tokenization failed for test_borderline_syntax.\n");
        return;
    }

    // Parse tokens into an AST
    ASTNode* tree = parse_program(tokens, token_count);
    if (!tree) {
        fprintf(stderr, "Error: Parsing failed for test_borderline_syntax.\n");
        free_tokens(tokens, token_count);
        return;
    }

    // Transpile the AST
    char* output = transpile(tree);
    if (!output) {
        fprintf(stderr, "Error: Transpilation failed for test_borderline_syntax.\n");
        free_ast(tree);
        free_tokens(tokens, token_count);
        return;
    }

    // Validate the transpiled output
    int result = (strstr(output, "int x = 2147483647;") != NULL);
    if (!result) {
        fprintf(stderr, "Error: Transpiled output does not match expected syntax.\n");
    }
    else {
        printf("test_borderline_syntax passed.\n");
    }

    // Free allocated memory
    free(output);
    free_ast(tree);
    free_tokens(tokens, token_count);
}


// Test generate_code_from_ir function
int test_generate_code_from_ir() {
    IRNode* ir_list = create_ir_node("int x = 10;", 1, 1, "let x = 10;");
    if (!ir_list) {
        fprintf(stderr, "Error: Failed to create IR node for test_generate_code_from_ir.\n");
        return 0;
    }
    append_ir_node(&ir_list, create_ir_node("printf(\"%d\", x);", 2, 1, "print(x);"));

    char* code = generate_code_from_ir(ir_list, "c");
    if (!code) {
        fprintf(stderr, "Error: Failed to generate code from IR.\n");
        free_ir_list(ir_list);
        return 0;
    }

    int result = (strstr(code, "int x = 10;") != NULL && strstr(code, "printf(\"%d\", x);") != NULL);
    if (!result) {
        fprintf(stderr, "Error: Generated code did not match expected output.\n");
    }
    else {
        printf("test_generate_code_from_ir passed.\n");
    }

    free(code);
    free_ir_list(ir_list);
    return result;
}


// test_transpile function
int test_transpile() {
    Token tokens[] = {
        {TOKEN_KEYWORD, "let", 1, 1},
        {TOKEN_IDENTIFIER, "x", 1, 5},
        {TOKEN_OPERATOR, "=", 1, 7},
        {TOKEN_LITERAL, "10", 1, 9},
        {TOKEN_SYMBOL, ";", 1, 11}
    };

    ASTNode* root = create_node(NODE_PROGRAM, tokens[0]);
    if (!root) {
        fprintf(stderr, "Error: Failed to allocate memory for root node.\n");
        return 0;
    }

    ASTNode* declaration = create_node(NODE_VARIABLE_DECLARATION, tokens[1]);
    if (!declaration) {
        fprintf(stderr, "Error: Failed to allocate memory for declaration node.\n");
        free_ast(root);
        return 0;
    }
    add_child(root, declaration);

    char* code = transpile(root);
    if (!code) {
        fprintf(stderr, "Error: Failed to transpile AST to code.\n");
        free_ast(root);
        return 0;
    }

    int result = (strstr(code, "int x = 10;") != NULL);
    if (!result) {
        fprintf(stderr, "Error: Transpiled code did not match expected output.\n");
    }
    else {
        printf("test_transpile passed.\n");
    }

    free(code);
    free_ast(root);
    return result;
}
void test_string_interpolation() {
    ASTNode node = {
        .type = NODE_STRING_INTERPOLATION,
        .token = {TOKEN_STRING, "Hello, ${name}!", 1, 1},
        .children = NULL,
        .child_count = 0
    };

    IRNode* ir_list = NULL;
    transpile_string_interpolation(&node, &ir_list);

    printf("Generated Code:\n%s\n", ir_list->code);
    // Expected: printf("Hello, %s\\n", name);

    free_ir_list(ir_list);
}

void test_transpile_function() {
    printf("Testing transpile_function...\n");
    ASTNode node;
    IRNode* ir_list = NULL;
    transpile_function(&node, &ir_list);
    assert(ir_list != NULL);
    printf("--> transpile_function passed\n");
}

void test_transpile_string_interpolation() {
    printf("Testing transpile_string_interpolation...\n");
    ASTNode node;
    IRNode* ir_list = NULL;
    transpile_string_interpolation(&node, &ir_list);
    assert(ir_list != NULL);
    printf("--> transpile_string_interpolation passed\n");
}