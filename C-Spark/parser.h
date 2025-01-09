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
    NodeType type;
    Token token;
    struct ASTNode** children;
    int child_count;
} ASTNode;

// Function declarations
ASTNode* parse_program(Token* tokens, int token_count);
ASTNode* parse_expression();
void free_ast(ASTNode* node);

#endif // PARSER_H
