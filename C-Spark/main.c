#include "lexer.h"
#include "parser.h"



// Main function
int main() {
    const char* code = "let x = (42 + (y * z));";

    // Tokenization
    int token_count = 0;
    Token* tokens = tokenize(code, &token_count);

    printf("Tokens:\n");
    print_tokens(tokens, token_count);

    // Parsing
    ASTNode* root = parse_program(tokens, token_count);

    printf("\nAbstract Syntax Tree (AST):\n");
    print_ast(root, 0); // Print the AST starting from the root

    // Cleanup
    free_ast(root);
    free_tokens(tokens, token_count);

    return 0;
}
