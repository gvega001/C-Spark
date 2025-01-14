#include "tokenizer.h"
#include <stdio.h>

// Debug function to print tokens
void print_tokens(Token* tokens, int count) {
    for (int i = 0; i < count; i++) {
        printf("Token(Type: %d, Value: '%s', Line: %d, Column: %d)\n",
            tokens[i].type, tokens[i].value, tokens[i].line, tokens[i].column);
    }
}

// Add other debugging utilities here if needed in the future
