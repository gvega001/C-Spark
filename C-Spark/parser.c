#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Global state
static Token* tokens;
static int token_count;
static int current_token;

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
    if (!parent || !child) return; // Safety check
    void* new_children = realloc(parent->children, (parent->child_count + 1) * sizeof(ASTNode*));
    if (!new_children) {
        fprintf(stderr, "Error: Memory allocation failed in add_child\n");
        exit(1);
    }
    parent->children = new_children;
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode* node) {
    if (!node) return; // Safety check
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    free(node->children);
    free(node);
}

// Error Recovery
void synchronize() {
    while (peek()->type != TOKEN_EOF && !match(TOKEN_SYMBOL, ";") && !match(TOKEN_SYMBOL, "{")) {
        advance();
    }
}

// Parsing Functions
ASTNode* parse_block();
ASTNode* parse_variable_declaration();
ASTNode* parse_function_definition();
ASTNode* parse_statement();
ASTNode* parse_for_statement();
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();

ASTNode* parse_program(Token* input_tokens, int input_token_count) {
    tokens = input_tokens;
    token_count = input_token_count;
    current_token = 0;

    ASTNode* root = create_node(NODE_PROGRAM, (Token) { TOKEN_EOF, "program", 0, 0 });
    while (peek()->type != TOKEN_EOF) {
        add_child(root, parse_statement());
    }

    return root;
}

ASTNode* parse_block() {
    if (!match(TOKEN_SYMBOL, "{")) {
        fprintf(stderr, "Error: Expected '{' at line %d, column %d\n", peek()->line, peek()->column);
        synchronize();
        return NULL;
    }

    ASTNode* block = create_node(NODE_BLOCK, (Token) { TOKEN_SYMBOL, "{", peek()->line, peek()->column });
    while (!match(TOKEN_SYMBOL, "}")) {
        if (peek()->type == TOKEN_EOF) {
            fprintf(stderr, "Error: Unterminated block at line %d, column %d\n", peek()->line, peek()->column);
            synchronize();
            return NULL;
        }
        add_child(block, parse_statement());
    }

    return block;
}

ASTNode* parse_variable_declaration() {
    Token* let_token = advance();
    Token* id = advance();
    if (!id || id->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected identifier after 'let'\n");
        synchronize();
        return NULL;
    }

    ASTNode* var_node = create_node(NODE_VARIABLE_DECLARATION, *let_token);
    add_child(var_node, create_node(NODE_ASSIGNMENT, *id));
    if (match(TOKEN_OPERATOR, "=")) {
        add_child(var_node, parse_expression());
    }
    else {
        fprintf(stderr, "Error: Expected '=' in variable declaration at line %d, column %d\n", id->line, id->column);
        synchronize();
        return NULL;
    }

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after variable declaration at line %d, column %d\n", id->line, id->column);
        synchronize();
        return NULL;
    }

    return var_node;
}

ASTNode* parse_function_definition() {
    Token* func_token = advance(); // Consume 'func'
    Token* id = advance();
    if (!id || id->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected function name after 'func'\n");
        synchronize();
        return NULL;
    }

    ASTNode* func_node = create_node(NODE_FUNCTION, *id);

    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        synchronize();
        return NULL;
    }

    ASTNode* params = create_node(NODE_PARAMETER_LIST, (Token) { TOKEN_SYMBOL, "(", id->line, id->column });
    while (!match(TOKEN_SYMBOL, ")")) {
        Token* param = advance();
        if (!param || param->type != TOKEN_IDENTIFIER) {
            fprintf(stderr, "Error: Expected parameter name in function definition\n");
            synchronize();
            return NULL;
        }
        add_child(params, create_node(NODE_VARIABLE_DECLARATION, *param));
        if (match(TOKEN_OPERATOR, "=")) {
            add_child(params, parse_expression());
        }
        if (!match(TOKEN_SYMBOL, ",")) break;
    }
    add_child(func_node, params);

    add_child(func_node, parse_block());
    return func_node;
}

ASTNode* parse_for_statement() {
    Token* for_token = advance();
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'for'\n");
        synchronize();
        return NULL;
    }

    ASTNode* for_node = create_node(NODE_FOR, *for_token);

    if (match(TOKEN_KEYWORD, "let")) {
        add_child(for_node, parse_variable_declaration());
    }
    else if (peek()->type == TOKEN_IDENTIFIER) {
        add_child(for_node, parse_expression());
    }
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after initialization in 'for' loop\n");
        synchronize();
        return NULL;
    }

    add_child(for_node, parse_expression());
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after condition in 'for' loop\n");
        synchronize();
        return NULL;
    }

    add_child(for_node, parse_expression());
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after update in 'for' loop\n");
        synchronize();
        return NULL;
    }

    add_child(for_node, parse_block());
    return for_node;
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
        fprintf(stderr, "Error: Unknown statement at line %d, column %d\n", peek()->line, peek()->column);
        synchronize();
        return NULL;
    }
}

// Expression Parsing
ASTNode* parse_expression() {
    ASTNode* left = parse_term();
    Token* t = peek();
    while (t && (strcmp(t->value, "+") == 0 || strcmp(t->value, "-") == 0)) {
        Token* op = advance();
        ASTNode* right = parse_term();
        ASTNode* op_node = create_node(NODE_EXPRESSION, *op);
        add_child(op_node, left);
        add_child(op_node, right);
        left = op_node;
        t = peek();
    }
    return left;
}

ASTNode* parse_term() {
    ASTNode* left = parse_factor();
    Token* t = peek();
    while (t && (strcmp(t->value, "*") == 0 || strcmp(t->value, "/") == 0)) {
        Token* op = advance();
        ASTNode* right = parse_factor();
        ASTNode* op_node = create_node(NODE_TERM, *op);
        add_child(op_node, left);
        add_child(op_node, right);
        left = op_node;
        t = peek();
    }
    return left;
}

ASTNode* parse_factor() {
    Token* t = peek();
    if (t->type == TOKEN_LITERAL || t->type == TOKEN_IDENTIFIER) {
        return create_node(NODE_FACTOR, *advance());
    }
    else if (match(TOKEN_SYMBOL, "(")) {
        ASTNode* node = parse_expression();
        if (!match(TOKEN_SYMBOL, ")")) {
            fprintf(stderr, "Error: Expected ')' after expression\n");
            synchronize();
        }
        return node;
    }
    else {
        fprintf(stderr, "Error: Unexpected token '%s' at line %d, column %d\n", t->value, t->line, t->column);
        synchronize();
        return NULL;
    }
}
