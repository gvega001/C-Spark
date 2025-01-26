#include "lexer_parser_tests.h"
#include <assert.h>

void run_test_case(const TestCase* test) {
    printf("Running test: %s\n", test->description);

    // Tokenization
    int token_count = 0;
    Token* tokens = tokenize(test->code, &token_count);

    if (!tokens) {
        printf("  Tokenization failed!\n\n");
        return;
    }

    printf("  Tokenization completed. Tokens:\n");
    for (int i = 0; i < token_count; i++) {
        printf("    Token[%d]: '%s', Type: %d, Line: %d, Column: %d\n",
            i, tokens[i].value, tokens[i].type, tokens[i].line, tokens[i].column);
    }

    // Parsing
    printf("  Starting parsing...\n");
    ASTNode* root = parse_program(tokens, token_count);

    if (root) {
        printf("  AST successfully created.\n");
        printf("  Abstract Syntax Tree:\n");
        print_ast(root, 0); // Print the AST

        // Debugging: Count child nodes and validate AST structure
        printf("  Analyzing AST structure...\n");
        if (root->child_count > 0) {
            printf("  Valid AST structure with %d child nodes.\n", root->child_count);
        }
        else {
            printf("  Warning: No child nodes in AST. Check parser logic.\n");
        }

        // Debugging: Traverse the AST and validate node types
        printf("  Traversing AST for detailed node analysis...\n");
        for (int i = 0; i < root->child_count; i++) {
            ASTNode* child = root->children[i];
            printf("    Child[%d]: NodeType = %d, Token = '%s', Line = %d, Column = %d\n",
                i, child->type, child->token.value, child->token.line, child->token.column);
        }

        // Free memory for the AST
        free_ast(root);
    }
    else {
        printf("  Parsing failed! No AST created.\n");
    }

    // Free tokens
    free_tokens(tokens, token_count);
    printf("\n");
}



void run_all_tests() {
    // Define test cases
    TestCase test_cases[] = {
        {"Simple Variable Declaration", "let x = 42;", 0},
        {"Multiple Declarations", "let x = 10; let y = 20;", 0},
        {"Print Statement", "print(x);", 0},
        {"If-Else Statement", "if (x > 0) { print(\"Positive\"); } else { print(\"Negative\"); }", 0},
        {"Nested Blocks", "{ let x = 10; { let y = 20; } }", 0},
        {"Expressions", "let z = (x + y) * 2;", 0},
        {"For Loop", "for (let i = 0; i < 10; i = i + 1) { print(i); }", 0},
        {"Valid For Loop", "for (let i = 0; i < 10; i = i + 1) { print(i); }"},
        {"Missing Semicolon", "for (let i = 0 i < 10; i = i + 1) { print(i); }"}, // Syntax error
        {"Infinite Loop", "for (;;) { print(\"Infinite\"); }"}, // Valid syntax
        {"Empty Body", "for (let i = 0; i < 10; i = i + 1);"}, // Valid, no block
        {"Unclosed Parenthesis", "for (let i = 0; i < 10; i = i + 1 { print(i); }"}, // Syntax error
        {"Only Initialization", "for (let i = 0;;) { print(i); }"}, // Valid syntax
        {"Functions", "func add(a, b) { return a + b; }", 0},
        {"Comments", "// Single-line comment\nlet x = 10; /* Multi-line comment */ let y = 20;", 0},
        {"Invalid Syntax", "let x 10;", 1},
        {"Simple Record", "record Point { x = 0; y = 0; };", 0},
        {"Empty Record", "record Empty { };", 0},
        {"Record with One Field", "record Circle { radius = 10; };", 0},
        {"Record Missing Field Value", "record Point { x = ; };", 1},
        {"Record Missing Closing Brace", "record Point { x = 0; y = 0;", 1},
        {"Record Without Fields", "record Empty", 1}
    };

    int test_count = sizeof(test_cases) / sizeof(test_cases[0]);

    // Run each test case
    for (int i = 0; i < test_count; i++) {
        run_test_case(&test_cases[i]);
    }
}


void test_deeply_nested_blocks() {
    const char* input = "{ { { { int x = 0; } } } }";
    int token_count = 0;
    Token* tokens = tokenize(input, &token_count);
    ASTNode* tree = parse_program(tokens, token_count);

    assert(tree != NULL);
    printf("test_deeply_nested_blocks passed.\n");
    free_ast(tree);
    free_tokens(tokens, token_count);
}

// Invalid syntax test
void test_invalid_syntax() {
    const char* input = "{ int x = ; }"; // Missing value
    int token_count = 0;

    Token* tokens = tokenize(input, &token_count);
    if (!tokens) {
        fprintf(stderr, "Error: Tokenization failed for invalid syntax test.\n");
        return;
    }

    ASTNode* tree = parse_program(tokens, token_count);
    if (tree) {
        fprintf(stderr, "Error: Parsing succeeded for invalid syntax when it should have failed.\n");
        free_ast(tree);
    }
    else {
        printf("test_invalid_syntax passed.\n");
    }

    free_tokens(tokens, token_count);
}
#include "lexer_parser_tests.h"
#include <assert.h>

// Test deeply nested records
void test_nested_records() {
    const char* input = "record Outer { x = 1; record Inner { y = 2; }; };";
    int token_count = 0;
    Token* tokens = tokenize(input, &token_count);

    ASTNode* tree = parse_program(tokens, token_count);
    assert(tree != NULL);
    printf("test_nested_records passed.\n");
    free_ast(tree);
    free_tokens(tokens, token_count);
}

// Test invalid record syntax
void test_invalid_record_syntax() {
    const char* input = "record Point { x = ; };"; // Missing value
    int token_count = 0;

    Token* tokens = tokenize(input, &token_count);
    ASTNode* tree = parse_program(tokens, token_count);

    if (!tree) {
        printf("test_invalid_record_syntax passed.\n");
    }
    else {
        fprintf(stderr, "Error: Invalid record syntax unexpectedly succeeded.\n");
        free_ast(tree);
    }

    free_tokens(tokens, token_count);
}

void run_debug_tests() {
    printf("Running debug tests...\n");
    test_nested_records();
    test_invalid_record_syntax();
    printf("All debug tests passed.\n");
}
