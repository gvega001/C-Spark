#pragma once
#ifndef LEXER_PARSER_TESTS_H
#define LEXER_PARSER_TESTS_H

#include "lexer.h"
#include "parser.h"
#ifndef LEXER_PARSER_TESTS_H
#define LEXER_PARSER_TESTS_H

void test_deeply_nested_blocks();
void test_invalid_syntax();
void test_borderline_syntax(); // Add this declaration

#endif // LEXER_PARSER_TESTS_H
// Test case structure
typedef struct {
    const char* description;
    const char* code;
    int should_fail; // 0 = success expected, 1 = failure expected
} TestCase;


// Function declarations
void run_test_case(const TestCase* test);
void run_all_tests();
void test_deeply_nested_blocks();
void test_invalid_syntax();
void test_nested_records();
void test_invalid_record_syntax();
void test_handle_unknown_character();
void test_tokenize_identifier();
void test_parse_function_parameters();
void run_debug_tests();
#endif // LEXER_PARSER_TESTS_H
