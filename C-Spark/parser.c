#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global state
static Token* tokens;
static int token_count;
static int current_token;

// Forward Declarations
ASTNode* parse_statement();
ASTNode* parse_block();
ASTNode* parse_variable_declaration();
ASTNode* parse_function_definition();
ASTNode* parse_for_statement();
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();

// Helper Functions
Token* advance() {
    return (current_token < token_count) ? &tokens[current_token++] : NULL;
}

Token* peek() {
    return (current_token < token_count) ? &tokens[current_token] : NULL;
}

int match(TokenType type, const char* value) {
    if (current_token < token_count && tokens[current_token].type == type &&
        (value == NULL || strcmp(tokens[current_token].value, value) == 0)) {
        advance();
        return 1;
    }
    return 0;
}

ASTNode* create_node(NodeType type, Token token) {
    ASTNode* node = malloc(sizeof(ASTNode));
    if (!node) {
        fprintf(stderr, "Error: Memory allocation failed in create_node\n");
        exit(1);
    }
    node->type = type;
    node->token = token;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

void add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) return;
    void* new_children = realloc(parent->children, (parent->child_count + 1) * sizeof(ASTNode*));
    if (!new_children) {
        fprintf(stderr, "Error: Memory allocation failed in add_child\n");
        exit(1);
    }
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    free(node->children);
    free(node);
}

// Parsing Functions
ASTNode* parse_program(Token* input_tokens, int input_token_count) {
    tokens = input_tokens;
    token_count = input_token_count;
    current_token = 0;

    ASTNode* root = create_node(NODE_PROGRAM, (Token) { TOKEN_EOF, "program", 0, 0 });
    while (peek()->type != TOKEN_EOF) {
        ASTNode* statement = parse_statement();
        if (statement) {
            add_child(root, statement);
        }
    }

    return root;
}

ASTNode* parse_statement() {
    if (match(TOKEN_KEYWORD, "let")) {
        return parse_variable_declaration();
    }
    else if (match(TOKEN_KEYWORD, "func")) {
        return parse_function_definition();
    }
    else if (match(TOKEN_KEYWORD, "for")) {
        return parse_for_statement();
    }
    else {
        fprintf(stderr, "Error: Unknown statement\n");
        return NULL;
    }
}

ASTNode* parse_variable_declaration() {
    // Stub implementation
    return create_node(NODE_VARIABLE_DECLARATION, (Token) { TOKEN_KEYWORD, "let", 0, 0 });
}

ASTNode* parse_function_definition() {
    // Stub implementation
    return create_node(NODE_FUNCTION, (Token) { TOKEN_KEYWORD, "func", 0, 0 });
}

ASTNode* parse_for_statement() {
    // Stub implementation
    return create_node(NODE_FOR, (Token) { TOKEN_KEYWORD, "for", 0, 0 });
}

ASTNode* parse_expression() {
    return create_node(NODE_EXPRESSION, (Token) { TOKEN_LITERAL, "expression", 0, 0 });
}

ASTNode* parse_term() {
    return create_node(NODE_TERM, (Token) { TOKEN_LITERAL, "term", 0, 0 });
}

ASTNode* parse_factor() {
    return create_node(NODE_FACTOR, (Token) { TOKEN_LITERAL, "factor", 0, 0 });
}
