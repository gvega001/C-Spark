#include <stdio.h>
#include "test_transpile_suite.h"
#include "lexer_parser_tests.h"
#include "tokenizer.h"

int main() {
    printf("Starting Test Suite for Lexer & Parser...\n");

    // Run all defined test cases
    run_all_tests();

    // Additional lexer and parser tests
    printf("Running additional Lexer & Parser tests...\n");
    test_deeply_nested_blocks();
    test_invalid_syntax();

    printf("Starting Test Suite for Transpiler...\n");

    // Transpiler-related tests
    run_test("Test safe_strdup", test_safe_strdup);
    run_test("Test append_code", test_append_code);
    run_test("Test IRNode creation", test_ir_node_creation);
    //run_test("Test transpile_to_ir", test_transpile_to_ir);
    run_test("Test unsupported node handling", test_unsupported_node_handling);
    run_test("Test generate_code_from_ir", test_generate_code_from_ir);
    //run_test("Test transpile", test_transpile);

    // Interdependent function and borderline syntax tests
    printf("Running additional Transpiler tests...\n");
    test_interdependent_functions();
    test_borderline_syntax();

    test_string_interpolation();
    printf("All tests completed.\n");
    return 0;
}