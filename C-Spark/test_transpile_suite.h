#ifndef TEST_TRANSPILE_SUITE_H
#define TEST_TRANSPILE_SUITE_H

// Function declarations for test cases
void run_test(const char* description, int (*test_function)());
int test_safe_strdup();
int test_append_code();
int test_ir_node_creation();
int test_transpile_to_ir();
int test_unsupported_node_handling();

#endif // TEST_TRANSPILE_SUITE_H
#pragma once
