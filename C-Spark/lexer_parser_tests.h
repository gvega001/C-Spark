#pragma once
#ifndef LEXER_PARSER_TESTS_H
#define LEXER_PARSER_TESTS_H

#include "lexer.h"
#include "parser.h"

// Test case structure
typedef struct {
    const char* description;
    const char* code;
} TestCase;

// Function declarations
void run_test_case(const TestCase* test);
void run_all_tests();

#endif // LEXER_PARSER_TESTS_H
