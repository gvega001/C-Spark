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

    // Parsing
    ASTNode* root = parse_program(tokens, token_count);

    if (root) {
        printf("  AST successfully created.\n");
        printf("  Abstract Syntax Tree:\n");
        print_ast(root, 0); // Print the AST

        // Add assertions for AST validation (simple example)
        assert(root->type == NODE_PROGRAM);
        if (root->child_count > 0) {
            printf("  Valid AST structure with %d child nodes.\n", root->child_count);
        }
        else {
            printf("  Warning: No child nodes in AST.\n");
        }

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
        {"Simple Variable Declaration", "let x = 42;"},
        {"Multiple Declarations", "let x = 10; let y = 20;"},
        {"Print Statement", "print(x);"},
        {"If-Else Statement", "if (x > 0) { print(\"Positive\"); } else { print(\"Negative\"); }"},
        {"Nested Blocks", "{ let x = 10; { let y = 20; } }"},
        {"Expressions", "let z = (x + y) * 2;"},
        {"For Loop", "for (let i = 0; i < 10; i = i + 1) { print(i); }"},
        {"Functions", "func add(a, b) { return a + b; }"},
        {"Comments", "// Single-line comment\nlet x = 10; /* Multi-line comment */ let y = 20;"},
        {"Invalid Syntax", "let x 10;"},
        {"Deeply Nested Blocks", "{ let a = 1; { let b = 2; { let c = 3; } } }"},
        {"Empty Block", "{ }"},
        {"Block Without Declaration", "{ print(\"Hello\"); }"},
        {"Invalid Block Syntax", "{ let x = 10; let y = 20; "}, // Missing closing '}'
        {"Mixed Nested Declarations", "{ let x = 5; if (x > 0) { let y = x + 1; } else { let z = -1; } }"}
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