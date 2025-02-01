#include <stdio.h>
#include "test_transpile_suite.h"
#include "lexer_parser_tests.h"
#include "test_error_handling.h"
#include "test_achievements.h"
#include "tokenizer.h"
#include "debugger.h"

void print_section_header(const char* title) {
    printf("\n========================================\n");
    printf(" %s\n", title);
    printf("========================================\n\n");
}

int main() {
    enable_debugging();  // Enable Debugging Mode
    printf("****************************************\n");
    printf("     C-Spark Automated Test Suite       \n");
    printf("****************************************\n\n");

    /*********************************************************/
    /*                  LEXER & PARSER TESTS                 */
    /*********************************************************/
    print_section_header("Lexer & Parser Tests");

    printf("Running all Lexer & Parser tests...\n");
    run_all_tests();

    printf("Running additional Lexer & Parser tests...\n");
    test_deeply_nested_blocks();
    test_invalid_syntax();

    /*********************************************************/
    /*                TRANSPILER TESTS                       */
    /*********************************************************/
    print_section_header("Transpiler Tests");

    run_test("Test safe_strdup", test_safe_strdup);
    run_test("Test append_code", test_append_code);
    run_test("Test IRNode creation", test_ir_node_creation);
    run_test("Test unsupported node handling", test_unsupported_node_handling);
    run_test("Test generate_code_from_ir", test_generate_code_from_ir);

    printf("Running additional Transpiler tests...\n");
    test_interdependent_functions();
    test_borderline_syntax();
    test_string_interpolation();

    /*********************************************************/
    /*                ERROR HANDLING TESTS                   */
    /*********************************************************/
    print_section_header("Error Handling Tests");

    run_test("Test report_error", test_report_error);
    run_test("Test log_error", test_log_error);

    /*********************************************************/
    /*                ACHIEVEMENT SYSTEM TESTS               */
    /*********************************************************/
    print_section_header("Gamification & Achievements");

    run_test("Test unlock_achievement", test_unlock_achievement);
    run_test("Test save and load achievements", test_save_and_load_achievements);

    /*********************************************************/
    /*                FINAL DEBUGGING TESTS                  */
    /*********************************************************/
    print_section_header("Final Debugging & Verification");

    run_debug_tests();

    printf("\n****************************************\n");
    printf("   ALL TESTS COMPLETED SUCCESSFULLY    \n");
    printf("****************************************\n\n");

    return 0;
}
