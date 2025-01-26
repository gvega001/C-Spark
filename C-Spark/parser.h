#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
typedef enum {
    TYPE_INT,          // Integer type
    TYPE_FLOAT,        // Floating-point type
    TYPE_STRING,       // String type
    TYPE_BOOL,         // Boolean type (true/false)
    TYPE_VOID,         // Void type (for functions with no return value)
    TYPE_CHAR,         // Single character
    TYPE_ARRAY,        // Array of elements (generic type for arrays)
    TYPE_OBJECT,       // Object or struct type
    TYPE_POINTER,      // Pointer type (for advanced usage)
    TYPE_ENUM,         // Enumerated type (future support for enums)
    TYPE_FUNCTION,     // Function type (for function pointers or higher-order functions)
    TYPE_STRUCT,  // For record types
    TYPE_CUSTOM,   // For future custom types
    TYPE_UNKNOWN,      // Unknown type (used for error handling)
    TYPE_ANY           // Dynamic type (used for dynamic typing or scripting-like features)
} DataType;

// AST Node Types
typedef enum {
    NODE_SWITCH,
    NODE_CASE,
    NODE_DEFAULT,
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
    NODE_FACTOR,
    NODE_LITERAL,                 // Literal values (e.g., numbers, strings)
    NODE_VARIABLE,                // Variables
    NODE_STRUCT,                  // Struct definitions
    NODE_STRING_INTERPOLATION     // String interpolation constructs
} NodeType;

// AST Node Structure
typedef struct ASTNode {
    NodeType type;                // Type of the AST node
    Token token;                  // Associated token for the node
    struct ASTNode** children;    // Dynamically allocated array of child nodes
    int child_count;              // Number of child nodes
    DataType inferred_type; // Add inferred type
} ASTNode;

// Parser function declarations
ASTNode* parse_program(Token* tokens, int token_count);
ASTNode* parse_statement();
ASTNode* parse_block();
ASTNode* parse_variable_declaration();
ASTNode* parse_function_definition();
ASTNode* parse_for_statement();
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();
ASTNode* parse_if_statement();       // Parse if statements
ASTNode* parse_print_statement();    // Parse print statements

// Memory management and debugging functions
void free_ast(ASTNode* node);        // Free the memory allocated for an AST
void print_ast(ASTNode* node, int depth); // Print the AST (for debugging)
ASTNode* parse_record_definition(); // Parse record definitions
ASTNode* parse_switch_statement();
ASTNode* parse_case_statement();
ASTNode* parse_default_case();

#endif // PARSER_H
