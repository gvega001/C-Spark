#include "lexer.h"
#include "parser.h"



// Main function
int main() {
    const char* code =
        "let x = 10;\n";
        //"print(x); // This is a comment\n"
       // "/* Multi-line comment */\n"
       //  "let y = \"Hello, world!\";\n";
       // "if (x == 10) {\n"
       // "  print(\"x is 10\");\n"
       // "}\n"
       // "else {\n"
       // "  print(\"x is not 10\");\n"
       // "}"

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
