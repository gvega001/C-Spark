#include "lexer_parser_tests.h"
#include <stdio.h>
#include <assert.h>
#include "lexer.h"
#include "parser.h"

// Run a single test case
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

// Run all test cases
void run_all_tests() {
    // Define test cases
    TestCase test_cases[] = {
        // Basic Statements
        {"Simple Variable Declaration", "let x = 42;", 0},
        {"Multiple Declarations", "let x = 10; let y = 20;", 0},
        {"Print Statement", "print(x);", 0},

        // Conditional Statements
        {"If-Else Statement", "if (x > 0) { print(\"Positive\"); } else { print(\"Negative\"); }", 0},

        // Switch Statement
        {"Simple Switch Statement", "switch (x) { case 1 { print(\"One\"); break; } case 2 { print(\"Two\"); break; } default { print(\"Other\"); } }", 0},

        // Loops - Standard Cases
        {"For Loop", "for (let i = 0; i < 10; i = i + 1) { print(i); }", 0},
        {"Valid For Loop", "for (let i = 0; i < 10; i = i + 1) { print(i); }", 0},
        {"Infinite Loop", "for (;;) { print(\"Infinite\"); }", 0},
        {"Empty Body", "for (let i = 0; i < 10; i = i + 1);", 0},

        // Loops - Edge Cases
        {"For Loop - No Initialization", "for (; i < 10; i = i + 1) { print(i); }", 0},
        {"For Loop - No Condition", "for (let i = 0;; i = i + 1) { print(i); }", 0},
        {"For Loop - Decrement", "for (let i = 10; i > 0; i = i - 1) { print(i); }", 0},
        {"For Loop - Custom Step", "for (let i = 0; i < 10; i = i + 2) { print(i); }", 0},

        // Loops - Syntax Errors
        {"Missing Semicolon", "for (let i = 0 i < 10; i = i + 1) { print(i); }", 1},
        {"Unclosed Parenthesis", "for (let i = 0; i < 10; i = i + 1 { print(i); }", 1},
        {"Only Initialization", "for (let i = 0;;) { print(i); }", 0},

        // Functions - Standard Cases
        {"Function Definition", "func add(a, b) { return a + b; }", 0},
        {"Function Call", "func add(a, b) { return a + b; } print(add(3,4));", 0},
        {"Recursive Function", "func fact(n) { if (n == 0) return 1; return n * fact(n - 1); }", 0},

        // Expressions
        {"Expressions", "let z = (x + y) * 2;", 0},

        // Comments
        {"Comments", "// Single-line comment\nlet x = 10; /* Multi-line comment */ let y = 20;", 0},

        // Error Handling - Syntax Issues
        {"Invalid Syntax", "let x 10;", 1},
        {"Unexpected Keyword", "return x + y;", 1},
        {"Undefined Variable", "let x = y + 2;", 1},
        {"Unmatched Closing Brace", "print(x); }", 1},

        // Records - Valid Cases
        {"Simple Record", "record Point { x = 0; y = 0; };", 0},
        {"Empty Record", "record Empty { };", 0},
        {"Record with One Field", "record Circle { radius = 10; };", 0},

        // Records - Invalid Cases
        {"Record Missing Field Value", "record Point { x = ; };", 1},
        {"Record Missing Closing Brace", "record Point { x = 0; y = 0;", 1},
        {"Record Without Fields", "record Empty", 1},

        // Records - Advanced
        {"Nested Record", "record Point { x = 0; y = 0; }; record Circle { center = Point(); radius = 5; };", 0},
        {"Record with Array", "record Grid { cells = [Point(), Point()]; };", 0}
    };


    int test_count = sizeof(test_cases) / sizeof(test_cases[0]);

    // Run each test case
    for (int i = 0; i < test_count; i++) {
        run_test_case(&test_cases[i]);
    }
}

// Test case for borderline syntax
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

// Run debug tests
void run_debug_tests() {
    printf("Running debug tests...\n");
    test_nested_records();
    test_invalid_record_syntax();
    printf("All debug tests passed.\n");
}

// Test handle_unknown_character
void test_handle_unknown_character() {
    printf("Testing handle_unknown_character...\n");
    char test_code[] = "@"; // Unexpected character
    Token tokens[10];
    int count = 0;
    handle_unknown_character(1, 1, test_code, 0, tokens, &count);
    assert(count == 0); // Should not generate a token
    printf("--> handle_unknown_character passed\n");
}

// Test tokenize_identifier
void test_tokenize_identifier() {
    printf("Testing tokenize_identifier...\n");
    char test_code[] = "variable";
    Token tokens[10];
    int count = 0;
    tokenize_identifier(1, 0, test_code, 0, tokens, &count);
    assert(tokens[0].type == TOKEN_IDENTIFIER);
    printf("--> tokenize_identifier passed\n");
}

// Test parse_function_parameters
void test_parse_function_parameters() {
    printf("Testing parse_function_parameters...\n");
    ParserState state;
    ASTNode* params = parse_function_parameters(&state);
    assert(params != NULL);
    printf("--> parse_function_parameters passed\n");
}