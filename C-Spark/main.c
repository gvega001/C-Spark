#include "lexer.h"
#include "parser.h"

// Test case structure
typedef struct {
    const char* description;
    const char* code;
} TestCase;

// Function to run a single test case
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
        free_ast(root);
    }
    else {
        printf("  Parsing failed! No AST created.\n");
    }

    // Free tokens
    free_tokens(tokens, token_count);
    printf("\n");
}

// Function to run all test cases
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
        {"Invalid Syntax", "let x 10;"}
    };

    int test_count = sizeof(test_cases) / sizeof(test_cases[0]);

    // Run each test case
    for (int i = 0; i < test_count; i++) {
        run_test_case(&test_cases[i]);
    }
}

int main() {
    run_all_tests();
    return 0;
}

