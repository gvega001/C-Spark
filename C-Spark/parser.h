#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// AST Node Types
typedef enum {
    NODE_PROGRAM,
    NODE_BLOCK,
    NODE_IF,
    NODE_ELSE,
    NODE_WHILE,
    NODE_FOR,
    NODE_FUNCTION,
    NODE_FUNCTION_CALL,
    NODE_PARAMETER_LIST,
    NODE_VARIABLE_DECLARATION,
    NODE_ASSIGNMENT,
    NODE_PRINT_STATEMENT,
    NODE_EXPRESSION,
    NODE_TERM,
    NODE_FACTOR
} NodeType;

// AST Node Structure
typedef struct ASTNode {
    NodeType type;               // Type of the AST node
    Token token;                 // Associated token for the node
    struct ASTNode** children;   // Dynamically allocated array of child nodes
    int child_count;             // Number of child nodes
} ASTNode;

// Parser function declarations
ASTNode* parse_program(Token* tokens, int token_count);    // Entry point for parsing
ASTNode* parse_statement();                                // Parse a single statement
ASTNode* parse_block();                                    // Parse a block of code (enclosed in {})
ASTNode* parse_variable_declaration();                    // Parse variable declarations
ASTNode* parse_function_definition();                     // Parse function definitions
ASTNode* parse_for_statement();                           // Parse for loop statements
ASTNode* parse_expression();                              // Parse general expressions
void free_ast(ASTNode* node);                              // Free the memory allocated for an AST

#endif // PARSER_H
