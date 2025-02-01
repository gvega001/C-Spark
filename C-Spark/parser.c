#include "parser.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"

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
ASTNode* parse_record_definition();
ASTNode* parse_switch_statement();
ASTNode* parse_case_statement();
ASTNode* parse_default_case();
void print_ast(ASTNode* node, int depth);

DataType resolve_type(const char* type_name) {
    if (strcmp(type_name, "int") == 0) return TYPE_INT;
    if (strcmp(type_name, "float") == 0) return TYPE_FLOAT;
    if (strcmp(type_name, "custom_type") == 0) return TYPE_CUSTOM;
    return TYPE_UNKNOWN; // Catch-all for unsupported types
}

int validate_types(DataType lhs, DataType rhs, const char* operator) {
    if ((lhs == TYPE_INT || lhs == TYPE_FLOAT) &&
        (rhs == TYPE_INT || rhs == TYPE_FLOAT)) {
        return 1; // Numeric operations are valid
    }
    if (lhs == TYPE_STRING && rhs == TYPE_STRING && strcmp(operator, "+") == 0) {
        return 1; // String concatenation is valid
    }
    if (lhs == TYPE_CUSTOM || rhs == TYPE_CUSTOM) {
        return 1; // Assume valid for now, can be extended
    }
    fprintf(stderr, "Error: Invalid types '%d' and '%d' for operator '%s'\n", lhs, rhs, operator);
    return 0;
}

// Centralized memory allocation check
void* check_memory_allocation(void* ptr, const char* context_message) {
    if (!ptr) {
        fprintf(stderr, "Error: Memory allocation failed in %s\n", context_message);
        exit(EXIT_FAILURE);
    }
    return ptr; // Return the pointer for chained usage
}

// Helper Functions
Token* advance() {
    return (current_token < token_count) ? &tokens[current_token++] : NULL;
}

Token* peek() {
    return (current_token >= 0 && current_token < token_count) ? &tokens[current_token] : NULL;
}

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

// Synchronize Function
void synchronize() {
    while (peek() && peek()->type != TOKEN_EOF) {
        // Synchronize by skipping tokens until a statement boundary is found
        if (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, ";") == 0) {
            advance(); // Skip past the semicolon
            break;
        }
        advance();
    }
}

ASTNode* create_node(NodeType type, Token token) {
    if (type < 0) {
        fprintf(stderr, "Error: Invalid node type\n");
        return NULL;
    }

    ASTNode* node = check_memory_allocation(safe_malloc(sizeof(ASTNode)), "create_node");

    node->type = type;
    node->token = token;
    node->children = NULL;
    node->child_count = 0;
    return node;
}

void add_child(ASTNode* parent, ASTNode* child) {
    if (!parent || !child) {
        fprintf(stderr, "Error: NULL parent or child in add_child\n");
        return;
    }

    parent->children = check_memory_allocation(
        safe_realloc(parent->children, (parent->child_count + 1) * sizeof(ASTNode*)),
        "add_child"
    );
    parent->children[parent->child_count++] = child;
}

void free_ast(ASTNode* node) {
    if (!node) return;
    for (int i = 0; i < node->child_count; i++) {
        free_ast(node->children[i]);
    }
    if (node->children) {
        free(node->children);
    }
    free(node);
}

void print_ast(ASTNode* node, int depth) {
    if (!node) return;
    for (int i = 0; i < depth; i++) printf("  ");
    printf("NodeType: %d, Token: '%s'\n", node->type, node->token.value);
    for (int i = 0; i < node->child_count; i++) {
        print_ast(node->children[i], depth + 1);
    }
}

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

// ------------------------------------------------------------
// Updated parse_statement()
// ------------------------------------------------------------
ASTNode* parse_statement() {
    if (!peek()) {
        fprintf(stderr, "Error: No more tokens to parse\n");
        synchronize(); // Recover and continue parsing
        return NULL;
    }

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
    else if (match(TOKEN_KEYWORD, "record")) {
        return parse_record_definition();
    }
    else if (match(TOKEN_KEYWORD, "switch")) {
        return parse_switch_statement();
    }
    else if (match(TOKEN_SYMBOL, "{")) {
        // Un-consume the token so that parse_block() can expect it.
        current_token--;
        return parse_block();
    }
    else if (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "(") == 0) {
        return parse_expression();
    }
    else {
        fprintf(stderr, "Error: Unexpected token '%s' at line %d, column %d.\n",
            peek()->value, peek()->line, peek()->column);
        synchronize(); // Skip to the next valid point
        return NULL;
    }
}

// ------------------------------------------------------------
// Block parsing
// ------------------------------------------------------------
static int process_block_statements(ASTNode* block) {
    while (peek() && strcmp(peek()->value, "}") != 0) {
        ASTNode* statement = parse_statement();
        if (statement) {
            add_child(block, statement);
        }
        else {
            fprintf(stderr, "Error: Invalid statement in block\n");
            return 0; // Indicate failure
        }
    }
    return 1; // Indicate success
}

ASTNode* parse_block() {
    if (!match(TOKEN_SYMBOL, "{")) {
        fprintf(stderr, "Error: Expected '{'\n");
        return NULL;
    }

    ASTNode* block = check_memory_allocation(create_node(NODE_BLOCK, (Token) { TOKEN_SYMBOL, "{", 0, 0 }), "parse_block");

    if (!process_block_statements(block)) {
        free_ast(block);
        return NULL;
    }

    if (!match(TOKEN_SYMBOL, "}")) {
        fprintf(stderr, "Error: Missing '}' at the end of block\n");
        free_ast(block);
        return NULL;
    }

    return block;
}

// ------------------------------------------------------------
// Variable Declaration Parsing
// ------------------------------------------------------------
static Token* parse_identifier() {
    if (match(TOKEN_IDENTIFIER, NULL)) {
        return &tokens[current_token - 1];
    }
    fprintf(stderr, "Error: Expected identifier after 'let'\n");
    return NULL;
}

static ASTNode* parse_initializer(ASTNode* var_decl) {
    if (match(TOKEN_OPERATOR, "=")) {
        ASTNode* expression = parse_expression();
        if (expression) {
            add_child(var_decl, expression);
            return var_decl;
        }
        fprintf(stderr, "Error: Invalid expression in variable declaration\n");
    }
    else {
        fprintf(stderr, "Error: Expected '=' in variable declaration\n");
    }
    // Do not free var_decl here—return NULL and let the caller clean up.
    return NULL;
}


static int parse_terminator(ASTNode* var_decl) {
    if (match(TOKEN_SYMBOL, ";")) {
        if (current_token < token_count) {
            Token* next = peek();
            if (next && next->type == TOKEN_SYMBOL && strcmp(next->value, ";") == 0) {
                fprintf(stderr, "Error: Unexpected extra semicolon after variable declaration at line %d, column %d\n",
                    tokens[current_token - 1].line, tokens[current_token - 1].column);
                return 0;
            }
        }
        return 1;
    }
    fprintf(stderr, "Error: Expected ';' after variable declaration at line %d, column %d\n",
        tokens[current_token - 1].line, tokens[current_token - 1].column);
    return 0;
}

ASTNode* parse_variable_declaration() {
    Token* identifier = parse_identifier();
    if (!identifier) return NULL;

    ASTNode* var_decl = check_memory_allocation(
        create_node(NODE_VARIABLE_DECLARATION, *identifier),
        "parse_variable_declaration"
    );

    if (!parse_initializer(var_decl)) {
        free_ast(var_decl);  // Free here if initializer fails.
        return NULL;
    }

    if (!parse_terminator(var_decl)) {
        free_ast(var_decl);
        return NULL;
    }

    return var_decl;
}

// ------------------------------------------------------------
// Function Definition Parsing
// ------------------------------------------------------------
static Token* parse_function_name() {
    if (match(TOKEN_IDENTIFIER, NULL)) {
        return &tokens[current_token - 1];
    }
    fprintf(stderr, "Error: Expected function name\n");
    return NULL;
}

int parse_function_parameters(ASTNode* func_def) {
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
            return 0; // Error parsing parameters
        }
    }
    return 1; // Parameters parsed successfully
}

static int match_symbol(const char* symbol, const char* error_message) {
    if (!match(TOKEN_SYMBOL, symbol)) {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }
    return 1;
}

ASTNode* parse_function_definition() {
    // Parse function name
    Token* identifier = parse_function_name();
    if (!identifier) return NULL;

    ASTNode* func_def = check_memory_allocation(create_node(NODE_FUNCTION, *identifier), "parse_function_definition");

    if (!match_symbol("(", "Error: Expected '(' after function name")) {
        free_ast(func_def);
        return NULL;
    }

    if (!parse_function_parameters(func_def)) {
        free_ast(func_def);
        return NULL;
    }

    if (!match_symbol(")", "Error: Expected ')' after parameters")) {
        free_ast(func_def);
        return NULL;
    }

    ASTNode* body = parse_block();
    if (!body) {
        fprintf(stderr, "Error: Expected valid block (e.g., '{ ... }') for function body\n");
        free_ast(func_def);
        return NULL;
    }

    add_child(func_def, body);
    return func_def;
}

// ------------------------------------------------------------
// For Statement Parsing
// ------------------------------------------------------------
ASTNode* parse_for_initialization() {
    // Allow an empty initialization if the next token is ';'
    if (peek() && peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, ";") == 0) {
        // Optionally, return a node representing an empty expression,
        // or simply return NULL and adjust parse_for_statement() accordingly.
        return create_node(NODE_EMPTY, *peek());
    }

    if (peek() && peek()->type == TOKEN_KEYWORD && strcmp(peek()->value, "let") == 0) {
        return parse_variable_declaration();
    }
    return parse_expression();
}


static int validate_symbol(const char* symbol, const char* error_message) {
    if (!match(TOKEN_SYMBOL, symbol)) {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }
    return 1;
}

static ASTNode* parse_required_expression(const char* error_message) {
    ASTNode* expr = parse_expression();
    if (!expr) {
        fprintf(stderr, "%s\n", error_message);
    }
    return expr;
}

ASTNode* parse_for_statement() {
    printf("Parsing 'for' loop at token %d\n", current_token);

    Token for_token = tokens[current_token - 1];
    ASTNode* for_node = create_node(NODE_FOR, for_token);
    if (!for_node) {
        fprintf(stderr, "Error: Memory allocation failed for 'for' node\n");
        return NULL;
    }

    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'for' at line %d, column %d\n",
            for_token.line, for_token.column);
        free_ast(for_node);
        return NULL;
    }

    ASTNode* init = parse_for_initialization();
    if (!init) {
        fprintf(stderr, "Error: Expected initialization in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, init);

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after initialization in 'for' loop at line %d, column %d\n",
            tokens[current_token - 1].line, tokens[current_token - 1].column);
        free_ast(for_node);
        return NULL;
    }

    ASTNode* condition = parse_expression();
    if (!condition) {
        fprintf(stderr, "Error: Expected condition in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, condition);

    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after condition in 'for' loop at line %d, column %d\n",
            tokens[current_token - 1].line, tokens[current_token - 1].column);
        free_ast(for_node);
        return NULL;
    }

    ASTNode* increment = parse_expression();
    if (!increment) {
        fprintf(stderr, "Error: Expected increment in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, increment);

    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after increment in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }

    ASTNode* body = parse_block();
    if (!body) {
        fprintf(stderr, "Error: Expected block in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, body);

    printf("For loop parsed successfully with %d child nodes.\n", for_node->child_count);
    return for_node;
}

// ------------------------------------------------------------
// Expression Parsing (including precedence and factors)
// ------------------------------------------------------------
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
        binary_op->inferred_type = TYPE_INT; // Default type

        if (strcmp(op_token->value, "+") == 0 || strcmp(op_token->value, "-") == 0) {
            if (lhs->inferred_type == TYPE_FLOAT || rhs->inferred_type == TYPE_FLOAT) {
                binary_op->inferred_type = TYPE_FLOAT;
            }
        }
        else if (strcmp(op_token->value, "&&") == 0 || strcmp(op_token->value, "||") == 0) {
            binary_op->inferred_type = TYPE_BOOL;
        }

        add_child(binary_op, lhs);
        add_child(binary_op, rhs);
        lhs = binary_op;
    }
    return lhs;
}

ASTNode* parse_expression() {
    return parse_expression_with_precedence(0);
}

ASTNode* parse_term() {
    return parse_factor();
}

static void parse_embedded_expressions(ASTNode* node) {
    const char* str = node->token.value;
    char buffer[128];
    int i = 0, j = 0;

    while (str[i] != '\0') {
        if (str[i] == '$' && str[i + 1] == '{') {
            i += 2; // Skip "${"
            while (str[i] != '}' && str[i] != '\0') {
                buffer[j++] = str[i++];
            }
            buffer[j] = '\0'; // Null-terminate
            if (str[i] == '}') i++;
            ASTNode* expr = parse_expression_with_precedence(0);
            add_child(node, expr);
        }
        else {
            i++;
        }
    }
}

static ASTNode* parse_grouped_expression() {
    advance(); // Consume '('
    ASTNode* expr = parse_expression();
    if (!expr) {
        fprintf(stderr, "Error: Invalid expression after '('\n");
        return NULL;
    }
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Missing ')' in grouped expression\n");
        free_ast(expr);
        return NULL;
    }
    return expr;
}

static ASTNode* parse_string_literal(Token* token) {
    if (strstr(token->value, "${")) { // String interpolation
        ASTNode* node = create_node(NODE_STRING_INTERPOLATION, *token);
        parse_embedded_expressions(node);
        advance();
        return node;
    }
    advance();
    return create_node(NODE_LITERAL, *token);
}

static ASTNode* parse_literal_or_identifier(Token* token) {
    advance();
    return create_node(NODE_FACTOR, *token);
}

ASTNode* parse_factor() {
    Token* token = peek();
    if (!token) {
        fprintf(stderr, "Error: Unexpected end of input in factor\n");
        return NULL;
    }

    if (token->type == TOKEN_SYMBOL && strcmp(token->value, "(") == 0) {
        return parse_grouped_expression();
    }

    if (token->type == TOKEN_STRING) {
        return parse_string_literal(token);
    }

    if (token->type == TOKEN_LITERAL || token->type == TOKEN_IDENTIFIER) {
        return parse_literal_or_identifier(token);
    }

    fprintf(stderr, "Error: Unexpected token '%s' in factor\n", token->value);
    advance();
    return NULL;
}

// ------------------------------------------------------------
// If Statement Parsing
// ------------------------------------------------------------
static ASTNode* parse_if_condition() {
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'if'\n");
        return NULL;
    }
    ASTNode* condition = parse_expression();
    if (!condition) {
        fprintf(stderr, "Error: Invalid condition in 'if' statement\n");
        return NULL;
    }
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after condition in 'if' statement\n");
        free_ast(condition);
        return NULL;
    }
    return condition;
}

static ASTNode* parse_if_block(const char* block_name) {
    ASTNode* block = parse_block();
    if (!block) {
        fprintf(stderr, "Error: Invalid %s block in 'if' statement\n", block_name);
    }
    return block;
}

ASTNode* parse_if_statement() {
    ASTNode* if_node = check_memory_allocation(
        create_node(NODE_IF, (Token) { TOKEN_KEYWORD, "if", 0, 0 }),
        "parse_if_statement"
    );

    ASTNode* condition = parse_if_condition();
    if (!condition) {
        free_ast(if_node);
        return NULL;
    }
    add_child(if_node, condition);

    ASTNode* if_block = parse_if_block("if");
    if (!if_block) {
        free_ast(if_node);
        return NULL;
    }
    add_child(if_node, if_block);

    if (match(TOKEN_KEYWORD, "else")) {
        ASTNode* else_block = parse_if_block("else");
        if (!else_block) {
            free_ast(if_node);
            return NULL;
        }
        add_child(if_node, else_block);
    }

    return if_node;
}

// ------------------------------------------------------------
// Print Statement Parsing
// ------------------------------------------------------------
static int expect_symbol(const char* symbol, const char* error_message) {
    if (!match(TOKEN_SYMBOL, symbol)) {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }
    return 1;
}

static ASTNode* parse_print_expression() {
    ASTNode* expression = parse_expression();
    if (!expression) {
        fprintf(stderr, "Error: Invalid expression in 'print' statement\n");
    }
    return expression;
}

ASTNode* parse_print_statement() {
    ASTNode* print_node = check_memory_allocation(
        create_node(NODE_PRINT_STATEMENT, tokens[current_token - 1]),
        "parse_print_statement"
    );

    if (!expect_symbol("(", "Error: Expected '(' after 'print'")) {
        free_ast(print_node);
        return NULL;
    }

    ASTNode* expression = parse_print_expression();
    if (!expression) {
        free_ast(print_node);
        return NULL;
    }
    add_child(print_node, expression);

    if (!expect_symbol(")", "Error: Expected ')' after 'print' expression")) {
        free_ast(print_node);
        return NULL;
    }

    if (!expect_symbol(";", "Error: Expected ';' after 'print' statement")) {
        free_ast(print_node);
        return NULL;
    }

    return print_node;
}

// ------------------------------------------------------------
// Record Definition Parsing
// (Assumes the 'record' keyword has already been matched.)
// ------------------------------------------------------------
static Token* parse_record_name(Token* record_token) {
    Token* name_token = advance();
    if (!name_token || name_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected record name after 'record' at line %d, column %d.\n",
            record_token->line, record_token->column);
        return NULL;
    }
    return name_token;
}

static int validate_opening_brace(const Token* name_token) {
    if (!match(TOKEN_SYMBOL, "{")) {
        fprintf(stderr, "Error: Expected '{' after record name '%s' at line %d, column %d.\n",
            name_token->value, name_token->line, name_token->column);
        return 0;
    }
    return 1;
}

static ASTNode* parse_record_field(const Token* name_token, const Token* record_token) {
    Token* field_name = advance();
    if (!field_name || field_name->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected field name in record '%s' at line %d, column %d.\n",
            name_token->value, record_token->line, record_token->column);
        return NULL;
    }

    if (!match(TOKEN_OPERATOR, "=")) {
        fprintf(stderr, "Error: Expected '=' after field name '%s' in record '%s' at line %d, column %d.\n",
            field_name->value, name_token->value, field_name->line, field_name->column);
        return NULL;
    }

    Token* field_value = advance();
    if (!field_value || (field_value->type != TOKEN_LITERAL && field_value->type != TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Error: Expected value for field '%s' in record '%s' at line %d, column %d.\n",
            field_name->value, name_token->value, field_name->line, field_name->column);
        return NULL;
    }

    ASTNode* field_node = create_node(NODE_VARIABLE_DECLARATION, *field_name);
    return field_node;
}

static int parse_record_fields(ASTNode* record_node, const Token* name_token, const Token* record_token) {
    while (!match(TOKEN_SYMBOL, "}")) {
        if (!peek()) {
            fprintf(stderr, "Error: Unterminated record definition for '%s' starting at line %d, column %d.\n",
                name_token->value, record_token->line, record_token->column);
            return 0;
        }

        ASTNode* field_node = parse_record_field(name_token, record_token);
        if (!field_node) {
            return 0;
        }
        add_child(record_node, field_node);
    }
    return 1;
}

ASTNode* parse_record_definition() {
    // 'record' keyword was already matched.
    Token* record_token = &tokens[current_token - 1];

    Token* name_token = parse_record_name(record_token);
    if (!name_token) return NULL;

    if (!validate_opening_brace(name_token)) return NULL;

    ASTNode* record_node = check_memory_allocation(create_node(NODE_STRUCT, *name_token), "parse_record_definition");

    if (!parse_record_fields(record_node, name_token, record_token)) {
        free_ast(record_node);
        return NULL;
    }

    printf("Record '%s' successfully parsed with %d fields.\n", name_token->value, record_node->child_count);
    return record_node;
}

// ------------------------------------------------------------
// Switch Statement Parsing
// (Assumes the 'switch' keyword has already been matched.)
// ------------------------------------------------------------
ASTNode* parse_switch_statement() {
    ASTNode* switch_node = create_node(NODE_SWITCH, (Token) { TOKEN_KEYWORD, "switch", 0, 0 });
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'switch'.\n");
        synchronize();
        return NULL;
    }

    ASTNode* condition = parse_expression();
    if (!condition) {
        fprintf(stderr, "Error: Invalid expression in 'switch'.\n");
        synchronize();
        return NULL;
    }
    add_child(switch_node, condition);

    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after 'switch' condition.\n");
        synchronize();
        return NULL;
    }

    if (!match(TOKEN_SYMBOL, "{")) {
        fprintf(stderr, "Error: Expected '{' to begin 'switch' body.\n");
        synchronize();
        return NULL;
    }

    // Use peek() for lookahead instead of match() in the loop condition.
    while (peek() && !(peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "}") == 0)) {
        ASTNode* case_node = parse_case_statement();
        if (case_node) {
            add_child(switch_node, case_node);
        }
        else {
            fprintf(stderr, "Warning: Skipping invalid case in 'switch'.\n");
            advance();
        }
    }

    if (!match(TOKEN_SYMBOL, "}")) {
        fprintf(stderr, "Error: Expected '}' after switch cases.\n");
        synchronize();
        return NULL;
    }

    return switch_node;
}

ASTNode* parse_case_statement() {
    // Check for 'case'
    if (peek() && peek()->type == TOKEN_KEYWORD && strcmp(peek()->value, "case") == 0) {
        advance(); // consume 'case'
        ASTNode* case_node = create_node(NODE_CASE, (Token) { TOKEN_KEYWORD, "case", 0, 0 });
        ASTNode* case_value = parse_expression();
        if (!case_value) {
            fprintf(stderr, "Error: Missing or invalid case value.\n");
            return NULL;
        }
        add_child(case_node, case_value);

        if (!match(TOKEN_COLON, ":")) {
            fprintf(stderr, "Error: Expected ':' after 'case' value.\n");
            return NULL;
        }

        // Loop until the next 'case', 'default', or closing '}' is encountered.
        while (peek() && !((peek()->type == TOKEN_KEYWORD &&
            (strcmp(peek()->value, "case") == 0 || strcmp(peek()->value, "default") == 0)) ||
            (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "}") == 0))) {
            ASTNode* statement = parse_statement();
            if (statement) {
                add_child(case_node, statement);
            }
            else {
                break;
            }
        }
        return case_node;
    }
    else if (peek() && peek()->type == TOKEN_KEYWORD && strcmp(peek()->value, "default") == 0) {
        return parse_default_case();
    }
    return NULL;
}

ASTNode* parse_default_case() {
    if (match(TOKEN_KEYWORD, "default")) {
        if (!match(TOKEN_COLON, ":")) {
            fprintf(stderr, "Error: Expected ':' after 'default'.\n");
            return NULL;
        }
        ASTNode* default_node = create_node(NODE_DEFAULT, (Token) { TOKEN_KEYWORD, "default", 0, 0 });

        while (peek() && !((peek()->type == TOKEN_KEYWORD &&
            (strcmp(peek()->value, "case") == 0 || strcmp(peek()->value, "default") == 0)) ||
            (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "}") == 0))) {
            ASTNode* statement = parse_statement();
            if (statement) {
                add_child(default_node, statement);
            }
            else {
                break;
            }
        }
        return default_node;
    }
    return NULL;
}
