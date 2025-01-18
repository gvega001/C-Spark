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

// Updated peek function
Token* peek() {
    return (current_token >= 0 && current_token < token_count) ? &tokens[current_token] : NULL;
}

// Updated match function
int match(TokenType type, const char* value) {
    if (current_token < 0 || current_token >= token_count) {
        fprintf(stderr, "Error: Invalid token access. current_token=%d, token_count=%d\n", current_token, token_count);
        return 0;
    }

    Token* current = &tokens[current_token];

    if (!current || !current->value) {
        fprintf(stderr, "Error: NULL token or token value at index %d\n", current_token);
        return 0;
    }

    if (current->type == type && (value == NULL || strcmp(current->value, value) == 0)) {
        advance();
        return 1;
    }

    return 0; // No match
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
    if (!peek()) {
        fprintf(stderr, "Error: No more tokens to parse\n");
        return NULL;
    }

    if (match(TOKEN_KEYWORD, "let")) {
        return parse_variable_declaration(); // Handles variable declarations
    }
    else if (match(TOKEN_KEYWORD, "func")) {
        return parse_function_definition(); // Handles function definitions
    }
    else if (match(TOKEN_KEYWORD, "for")) {
        return parse_for_statement(); // Handles for loops
    }
    else if (match(TOKEN_KEYWORD, "if")) {
        return parse_if_statement(); // Handles if statements
    }
    else if (match(TOKEN_KEYWORD, "print")) {
        return parse_print_statement(); // Handles print statements
    }
    else if (match(TOKEN_SYMBOL, "{")) {
        current_token--; // Let parse_block handle blocks starting with '{'
        return parse_block();
    }
    else if (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "(") == 0) {
        return parse_expression(); // Handles standalone expressions
    }

    // Error handling for unknown statements
    fprintf(stderr, "Error: Unknown statement '%s' (type=%d)\n", peek()->value, peek()->type);
    advance(); // Skip invalid token to recover
    return NULL;
}




// Updated parse_block function
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
        } else {
            fprintf(stderr, "Error: Invalid statement in block\n");
            free_ast(block);
            return NULL;
        }
    }

    if (!match(TOKEN_SYMBOL, "}")) {
        fprintf(stderr, "Error: Missing '}' at the end of block\n");
        free_ast(block);
        return NULL;
    }

    return block;
}


ASTNode* parse_variable_declaration() {
    Token* identifier = NULL;

    // Match and validate identifier after 'let'
    if (match(TOKEN_IDENTIFIER, NULL)) {
        identifier = &tokens[current_token - 1];
    }
    else {
        fprintf(stderr, "Error: Expected identifier after 'let'\n");
        return NULL;
    }

    // Create the variable declaration node
    ASTNode* var_decl = create_node(NODE_VARIABLE_DECLARATION, *identifier);

    // Match and validate the '=' operator
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
    else {
        fprintf(stderr, "Error: Expected '=' in variable declaration\n");
        free_ast(var_decl);
        return NULL;
    }

    // Match and validate exactly one ';' at the end
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after variable declaration\n");
        free_ast(var_decl);
        return NULL;
    }

    // Ensure there are no unexpected tokens
    Token* next = peek();
    if (next && next->type == TOKEN_SYMBOL && strcmp(next->value, ";") == 0) {
        fprintf(stderr, "Error: Unexpected extra semicolon after variable declaration\n");
        free_ast(var_decl);
        return NULL;
    }

    return var_decl;
}

ASTNode* parse_function_definition() {
    // Match and validate function name
    Token* identifier = NULL;
    if (match(TOKEN_IDENTIFIER, NULL)) {
        identifier = &tokens[current_token - 1];
    }
    else {
        fprintf(stderr, "Error: Expected function name\n");
        return NULL;
    }

    ASTNode* func_def = create_node(NODE_FUNCTION, *identifier);

    // Match opening parenthesis
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after function name\n");
        free_ast(func_def);
        return NULL;
    }

    // Parse parameters
    while (peek() && strcmp(peek()->value, ")") != 0) {
        if (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, ",") == 0) {
            advance(); // Skip comma
            continue;
        }
        if (peek()->type == TOKEN_IDENTIFIER) {
            ASTNode* param = create_node(NODE_PARAMETER_LIST, *advance());
            add_child(func_def, param);
        }
        else {
            fprintf(stderr, "Error: Expected parameter name, got '%s'\n", peek()->value);
            free_ast(func_def);
            return NULL;
        }
    }


    // Match closing parenthesis
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after parameters\n");
        free_ast(func_def);
        return NULL;
    }

    // Parse function body
    ASTNode* body = parse_block();
    if (!body) {
        fprintf(stderr, "Error: Expected valid block (e.g., '{ ... }') for function body\n");
        free_ast(func_def);
        return NULL;
    }

    add_child(func_def, body);
    return func_def;
}

ASTNode* parse_for_statement() {
    // Create the 'for' node
    ASTNode* for_node = create_node(NODE_FOR, (Token) { TOKEN_KEYWORD, "for", 0, 0 });

    // Match '('
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'for'\n");
        free_ast(for_node);
        return NULL;
    }

    // Parse initialization (variable declaration or expression)
    ASTNode* init = NULL;
    if (peek()->type == TOKEN_KEYWORD && strcmp(peek()->value, "let") == 0) {
        init = parse_variable_declaration();
    }
    else {
        init = parse_expression();
    }

    if (!init) {
        fprintf(stderr, "Error: Expected initialization in 'for' loop\n");
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, init);

    // Match ';'
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after 'for' initialization\n");
        free_ast(for_node);
        return NULL;
    }

    // Parse condition
    ASTNode* condition = parse_expression();
    if (!condition) {
        fprintf(stderr, "Error: Expected condition in 'for' loop\n");
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, condition);

    // Match another ';'
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after 'for' condition\n");
        free_ast(for_node);
        return NULL;
    }

    // Parse increment
    ASTNode* increment = parse_expression();
    if (!increment) {
        fprintf(stderr, "Error: Expected increment in 'for' loop\n");
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, increment);

    // Match closing ')'
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after 'for' increment\n");
        free_ast(for_node);
        return NULL;
    }

    // Parse loop body
    ASTNode* body = parse_block();
    if (!body) {
        fprintf(stderr, "Error: Expected block in 'for' loop\n");
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, body);

    return for_node;
}


int get_precedence(Token* token) {
    if (!token) return -1;
    if (token->type == TOKEN_OPERATOR) {
        if (strcmp(token->value, "*") == 0 || strcmp(token->value, "/") == 0) return 3;
        if (strcmp(token->value, "+") == 0 || strcmp(token->value, "-") == 0) return 2;
        if (strcmp(token->value, "==") == 0 || strcmp(token->value, "!=") == 0) return 1;
        if (strcmp(token->value, "<") == 0 || strcmp(token->value, "<=") == 0 ||
            strcmp(token->value, ">") == 0 || strcmp(token->value, ">=") == 0) return 1;
    }
    return -1; // Lowest precedence
}

ASTNode* parse_expression_with_precedence(int min_precedence) {
    ASTNode* lhs = parse_factor();
    if (!lhs) return NULL;

    while (peek() && get_precedence(peek()) >= min_precedence) {
        Token* op_token = advance();
        ASTNode* rhs = parse_expression_with_precedence(get_precedence(op_token) + 1);
        if (!rhs) {
            fprintf(stderr, "Error: Invalid right-hand side in expression\n");
            free_ast(lhs);
            return NULL;
        }

        ASTNode* binary_op = create_node(NODE_EXPRESSION, *op_token);
        add_child(binary_op, lhs);
        add_child(binary_op, rhs);
        lhs = binary_op;
    }

    return lhs;
}


// Updated parse_expression
ASTNode* parse_expression() {
    return parse_expression_with_precedence(0);
}


ASTNode* parse_term() {
    return parse_factor();
}

ASTNode* parse_factor() {
    Token* token = peek();
    if (!token) {
        fprintf(stderr, "Error: Unexpected end of input in factor\n");
        return NULL;
    }
    // Handle grouping: '(' expression ')'
    if (token->type == TOKEN_SYMBOL && strcmp(token->value, "(") == 0) {
        // Consume '('
        advance();

        // Parse sub-expression
        ASTNode* expr = parse_expression();
        if (!expr) {
            fprintf(stderr, "Error: Invalid sub-expression after '('\n");
            return NULL;
        }

        // Expect closing ')'
        if (!match(TOKEN_SYMBOL, ")")) {
            fprintf(stderr, "Error: Missing ')' in grouped expression\n");
            free_ast(expr);
            return NULL;
        }

        return expr; // Return the grouped expression as the factor
    }
    if (token->type == TOKEN_LITERAL || token->type == TOKEN_IDENTIFIER || token->type == TOKEN_STRING) {
        advance();
        return create_node(NODE_FACTOR, *token);
    }

    fprintf(stderr, "Error: Unexpected token '%s' in factor\n", token->value);
    advance(); // Skip invalid token
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
    // Create the node using the previously matched token
    ASTNode* print_node = create_node(
        NODE_PRINT_STATEMENT,
        tokens[current_token - 1] // Use the 'print' token we just advanced over
    );

    // Now expect '('
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'print'\n");
        free_ast(print_node);
        return NULL;
    }

    // Parse the expression inside the parentheses
    ASTNode* expression = parse_expression();
    if (!expression) {
        fprintf(stderr, "Error: Invalid expression in 'print' statement\n");
        free_ast(print_node);
        return NULL;
    }
    add_child(print_node, expression);

    // Expect ')'
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after 'print' expression\n");
        free_ast(print_node);
        return NULL;
    }

    // Expect ';'
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after 'print' statement\n");
        free_ast(print_node);
        return NULL;
    }

    return print_node;
}
