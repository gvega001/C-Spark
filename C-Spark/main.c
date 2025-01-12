#include <stdio.h>
#include "test_transpile_suite.h"
#include "lexer_parser_tests.h"

int main() {
    printf("Starting Test Suite for Lexer&Parser...\n");
    run_all_tests();
    printf("Starting Test Suite for Transpiler...\n");

    // Run individual tests
    run_test("Test safe_strdup", test_safe_strdup);
    run_test("Test append_code", test_append_code);
    run_test("Test IRNode creation", test_ir_node_creation);
   // run_test("Test transpile_to_ir", test_transpile_to_ir);
    run_test("Test unsupported node handling", test_unsupported_node_handling);

    printf("All tests completed.\n");
    return 0;
}
