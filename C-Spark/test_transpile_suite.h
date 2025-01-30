#ifndef TEST_TRANSPILE_SUITE_H
#define TEST_TRANSPILE_SUITE_H

// Function declarations for test cases
void run_test(const char* description, int (*test_function)());
int test_safe_strdup();
int test_append_code();
int test_ir_node_creation();
int test_transpile_to_ir();
int test_unsupported_node_handling();
int test_generate_code_from_ir();
int test_transpile();
void test_interdependent_functions();
void test_transpile_function();
void test_transpile_string_interpolation();

#endif // TEST_TRANSPILE_SUITE_H
