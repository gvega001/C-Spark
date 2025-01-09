#include "lexer.h"
#include "parser.h"
// Main function
int main() {
    const char* code = "let x = 10;\nprint(x); // This is a comment\n/* Multi-line comment */\nlet y = \"Hello, world!\";\nif (x == 10) {\n  print(\"x is 10\");\n}\nelse {\n  print(\"x is not 10\");\n}";
    int token_count = 0;
    Token* tokens = tokenize(code, &token_count);

    printf("Tokens:\n");
    print_tokens(tokens, token_count);

    free_tokens(tokens, token_count);  // Free allocated memory
    return 0;
}
