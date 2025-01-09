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
ASTNode* parse_if_statement();
ASTNode* parse_print_statement();
void print_ast(ASTNode* node, int depth);

// Helper Functions
Token* advance() {
    return (current_token < token_count) ? &tokens[current_token++] : NULL;
}

Token* peek() {
    return (current_token < token_count) ? &tokens[current_token] : NULL;
}
int match(TokenType type, const char* value) {
    if (current_token < token_count) {
        Token* current = &tokens[current_token];
        printf("match(): Current token index=%d, token='%s' (type=%d), expected type=%d, value='%s'\n",
            current_token, current->value, current->type, type, value ? value : "<any>");

        if (current->type == type && (value == NULL || strcmp(current->value, value) == 0)) {
            advance();
            return 1;
        }
    }

    return 0; // No match, no advancement
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

// Function to print the AST (for debugging purposes)
void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("NodeType: %d, Token: '%s'\n", node->type, node->token.value);
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

// Parsing Functions
ASTNode* parse_program(Token* input_tokens, int input_token_count) {
    tokens = input_tokens;
    token_count = input_token_count;
    current_token = 0;

    ASTNode* root = create_node(NODE_PROGRAM, (Token) { TOKEN_EOF, "program", 0, 0 });
    while (peek() && peek()->type != TOKEN_EOF) {
        ASTNode* statement = parse_statement();
        if (statement) {
            add_child(root, statement);
        }
    }
    return root;
}
ASTNode* parse_statement() {
    printf("Parsing statement. Current token: '%s' (type=%d)\n", tokens[current_token].value, tokens[current_token].type);

    if (match(TOKEN_KEYWORD, "let")) {
        return parse_variable_declaration();
    }
    else if (match(TOKEN_KEYWORD, "func")) {
        return parse_function_definition();
    }
    else if (match(TOKEN_KEYWORD, "for")) {
        return parse_for_statement();
    }
    else if (match(TOKEN_KEYWORD, "if")) {
        return parse_if_statement();
    }
    else if (match(TOKEN_KEYWORD, "print")) {
        return parse_print_statement();
    }
    else if (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "(") == 0) {
        // Handle as an expression
        return parse_expression();
    }
    else if (match(TOKEN_SYMBOL, "{")) {
        current_token--; // Rewind to allow `parse_block` to handle the '{'
        return parse_block();
    }
    else {
        fprintf(stderr, "Error: Unknown statement '%s' (type=%d)\n",
            tokens[current_token].value, tokens[current_token].type);
        return NULL;
    }
}


ASTNode* parse_block() {
    if (!match(TOKEN_SYMBOL, "{")) {
        fprintf(stderr, "Error: Expected '{'\n");
        return NULL;
    }

    ASTNode* block = create_node(NODE_BLOCK, (Token) { TOKEN_SYMBOL, "{", 0, 0 });

    while (peek() && strcmp(peek()->value, "}") != 0) {
        ASTNode* statement = parse_statement();
        if (statement) {
            add_child(block, statement);
        }
        else {
            free_ast(block);
            return NULL;
        }
    }

    if (!match(TOKEN_SYMBOL, "}")) {
        fprintf(stderr, "Error: Missing '}' at the end of a block\n");
        free_ast(block);
        return NULL;
    }

    return block;
}

ASTNode* parse_variable_declaration() {
    Token* identifier = NULL;
    if (match(TOKEN_IDENTIFIER, NULL)) {
        identifier = &tokens[current_token - 1];
    }
    else {
        fprintf(stderr, "Error: Expected identifier after 'let'\n");
        return NULL;
    }

    ASTNode* var_decl = create_node(NODE_VARIABLE_DECLARATION, *identifier);

    if (match(TOKEN_OPERATOR, "=")) {
        ASTNode* expression = parse_expression();
        if (expression) {
            add_child(var_decl, expression);
        }
        else {
            fprintf(stderr, "Error: Invalid expression in variable declaration\n");
            free_ast(var_decl);
            return NULL;
        }
    }

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after variable declaration\n");
        free_ast(var_decl);
        return NULL;
    }

    return var_decl;
}

ASTNode* parse_function_definition() {
    Token* identifier = NULL;
    if (match(TOKEN_IDENTIFIER, NULL)) {
        identifier = &tokens[current_token - 1];
    }
    else {
        fprintf(stderr, "Error: Expected function name\n");
        return NULL;
    }

    ASTNode* func_def = create_node(NODE_FUNCTION, *identifier);

    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        free_ast(func_def);
        return NULL;
    }

    // Parse parameters (skipped for now)
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after parameters\n");
        free_ast(func_def);
        return NULL;
    }

    ASTNode* body = parse_block();
    if (!body) {
        fprintf(stderr, "Error: Invalid function body\n");
        free_ast(func_def);
        return NULL;
    }

    add_child(func_def, body);
    return func_def;
}

ASTNode* parse_for_statement() {
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'for'\n");
        return NULL;
    }

    ASTNode* for_node = create_node(NODE_FOR, (Token) { TOKEN_KEYWORD, "for", 0, 0 });

    ASTNode* init = parse_expression();
    if (init) add_child(for_node, init);

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after for initialization\n");
        free_ast(for_node);
        return NULL;
    }

    ASTNode* condition = parse_expression();
    if (condition) add_child(for_node, condition);

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after for condition\n");
        free_ast(for_node);
        return NULL;
    }

    ASTNode* increment = parse_expression();
    if (increment) add_child(for_node, increment);

    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after for increment\n");
        free_ast(for_node);
        return NULL;
    }

    ASTNode* body = parse_block();
    if (body) add_child(for_node, body);

    return for_node;
}

ASTNode* parse_expression() {
    Token* token = peek();

    if (!token) {
        fprintf(stderr, "Error: Unexpected end of input in expression\n");
        return NULL;
    }

    if (token->type == TOKEN_LITERAL || token->type == TOKEN_IDENTIFIER) {
        advance();
        return create_node(NODE_EXPRESSION, *token);
    }
    else if (token->type == TOKEN_SYMBOL && strcmp(token->value, "(") == 0) {
        return parse_factor(); // Delegate to factor for grouped expressions
    }

    fprintf(stderr, "Error: Unexpected token '%s' in expression\n", token->value);
    return NULL;
}


ASTNode* parse_term() {
    return parse_factor();
}

ASTNode* parse_factor() {
    Token* token = peek();
    if (!token) {
        fprintf(stderr, "Error: Unexpected end of input\n");
        return NULL;
    }

    if (match(TOKEN_SYMBOL, "(")) {
        ASTNode* expr = parse_expression(); // Parse the grouped expression
        if (!match(TOKEN_SYMBOL, ")")) {
            fprintf(stderr, "Error: Missing ')' in grouped expression\n");
            free_ast(expr);
            return NULL;
        }
        return expr; // Return the grouped expression node
    }
    else if (token->type == TOKEN_LITERAL || token->type == TOKEN_IDENTIFIER) {
        advance();
        return create_node(NODE_FACTOR, *token);
    }

    fprintf(stderr, "Error: Unexpected token '%s'\n", token->value);
    return NULL;
}

ASTNode* parse_if_statement() {
    ASTNode* if_node = create_node(NODE_IF, (Token) { TOKEN_KEYWORD, "if", 0, 0 });

    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'if'\n");
        free_ast(if_node);
        return NULL;
    }

    ASTNode* condition = parse_expression();
    if (!condition) {
        fprintf(stderr, "Error: Invalid condition in 'if' statement\n");
        free_ast(if_node);
        return NULL;
    }
    add_child(if_node, condition);

    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after condition in 'if' statement\n");
        free_ast(if_node);
        return NULL;
    }

    ASTNode* if_block = parse_block();
    if (!if_block) {
        fprintf(stderr, "Error: Invalid block in 'if' statement\n");
        free_ast(if_node);
        return NULL;
    }
    add_child(if_node, if_block);

    if (match(TOKEN_KEYWORD, "else")) {
        ASTNode* else_block = parse_block();
        if (!else_block) {
            fprintf(stderr, "Error: Invalid block in 'else' statement\n");
            free_ast(if_node);
            return NULL;
        }
        add_child(if_node, else_block);
    }

    return if_node;
}

ASTNode* parse_print_statement() {
    if (!match(TOKEN_KEYWORD, "print")) {
        fprintf(stderr, "Error: Expected 'print'\n");
        return NULL;
    }

    ASTNode* print_node = create_node(NODE_PRINT_STATEMENT, (Token) { TOKEN_KEYWORD, "print", 0, 0 });

    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'print'\n");
        free_ast(print_node);
        return NULL;
    }

    ASTNode* expression = parse_expression();
    if (!expression) {
        fprintf(stderr, "Error: Invalid expression in 'print' statement\n");
        free_ast(print_node);
        return NULL;
    }
    add_child(print_node, expression);

    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after 'print' expression\n");
        free_ast(print_node);
        return NULL;
    }

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after 'print' statement\n");
        free_ast(print_node);
        return NULL;
    }

    return print_node;
}
