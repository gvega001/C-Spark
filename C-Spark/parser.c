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

        // Skip token to recover
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
        return parse_record_definition(); // Handles record definitions
    }
    else if (match(TOKEN_SYMBOL, "{")) {
        current_token--;
        return parse_block();
    }
    else if (peek()->type == TOKEN_SYMBOL && strcmp(peek()->value, "(") == 0) {
        return parse_expression();
    }else if (match(TOKEN_KEYWORD, "record")) {
    return parse_record_definition(); // Handles record definitions
}
    else {
        fprintf(stderr, "Error: Unexpected token '%s' at line %d, column %d.\n",
            peek()->value, peek()->line, peek()->column);
        synchronize(); // Skip to the next valid point
        return NULL;
    }
}

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

    // Allocate memory for block node and validate it
    ASTNode* block = check_memory_allocation(create_node(NODE_BLOCK, (Token) { TOKEN_SYMBOL, "{", 0, 0 }), "parse_block");

    // Process block statements
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

// Function to parse an identifier
static Token* parse_identifier() {
    if (match(TOKEN_IDENTIFIER, NULL)) {
        return &tokens[current_token - 1];
    }
    fprintf(stderr, "Error: Expected identifier after 'let'\n");
    return NULL;
}

// Function to parse the initializer (expression after '=')
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
    free_ast(var_decl);
    return NULL;
}

// Function to parse the terminating semicolon
static int parse_terminator(ASTNode* var_decl) {
    if (match(TOKEN_SYMBOL, ";")) {
        Token* next = peek();
        if (next && next->type == TOKEN_SYMBOL && strcmp(next->value, ";") == 0) {
            fprintf(stderr, "Error: Unexpected extra semicolon after variable declaration\n");
            return 0;
        }
        return 1;
    }
    fprintf(stderr, "Error: Expected ';' after variable declaration\n");
    return 0;
}

// Refactored parse_variable_declaration function
ASTNode* parse_variable_declaration() {
    Token* identifier = parse_identifier();
    if (!identifier) return NULL;

    ASTNode* var_decl = check_memory_allocation(
        create_node(NODE_VARIABLE_DECLARATION, *identifier),
        "parse_variable_declaration"
    );

    if (!parse_initializer(var_decl)) {
        return NULL;
    }

    if (!parse_terminator(var_decl)) {
        free_ast(var_decl);
        return NULL;
    }

    return var_decl;
}


// Helper function to parse the function name
static Token* parse_function_name() {
    if (match(TOKEN_IDENTIFIER, NULL)) {
        return &tokens[current_token - 1];
    }
    fprintf(stderr, "Error: Expected function name\n");
    return NULL;
}

// Helper function to parse parameters
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

// Helper function to validate and match symbols
static int match_symbol(const char* symbol, const char* error_message) {
    if (!match(TOKEN_SYMBOL, symbol)) {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }
    return 1;
}

// Refactored parse_function_definition function
ASTNode* parse_function_definition() {
    // Parse function name
    Token* identifier = parse_function_name();
    if (!identifier) return NULL;

    // Allocate memory for function definition node
    ASTNode* func_def = check_memory_allocation(create_node(NODE_FUNCTION, *identifier), "parse_function_definition");

    // Match opening parenthesis
    if (!match_symbol("(", "Error: Expected '(' after function name")) {
        free_ast(func_def);
        return NULL;
    }

    // Parse function parameters
    if (!parse_function_parameters(func_def)) {
        free_ast(func_def);
        return NULL;
    }

    // Match closing parenthesis
    if (!match_symbol(")", "Error: Expected ')' after parameters")) {
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

// Helper function to parse the 'for' initialization
ASTNode* parse_for_initialization() {
    Token* current = peek();
    if (current->type == TOKEN_KEYWORD && strcmp(current->value, "let") == 0) {
        // Parse variable declaration
        return parse_variable_declaration();
    }
    // Parse general expression if no 'let'
    return parse_expression();
}


// Helper function to parse and validate a specific symbol
static int validate_symbol(const char* symbol, const char* error_message) {
    if (!match(TOKEN_SYMBOL, symbol)) {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }
    return 1;
}

// Helper function to parse a required expression
static ASTNode* parse_required_expression(const char* error_message) {
    ASTNode* expr = parse_expression();
    if (!expr) {
        fprintf(stderr, "%s\n", error_message);
    }
    return expr;
}

// Refactored parse_for_statement function
ASTNode* parse_for_statement() {
    printf("Parsing 'for' loop at token %d\n", current_token);

    // Create 'for' node
    Token for_token = tokens[current_token - 1];
    ASTNode* for_node = create_node(NODE_FOR, for_token);
    if (!for_node) {
        fprintf(stderr, "Error: Memory allocation failed for 'for' node\n");
        return NULL;
    }

    // Match '('
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'for' at line %d, column %d\n",
            for_token.line, for_token.column);
        free_ast(for_node);
        return NULL;
    }

    // Parse initialization
    ASTNode* init = parse_for_initialization();
    if (!init) {
        fprintf(stderr, "Error: Expected initialization in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, init);

    // Match first ';'
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after initialization in 'for' loop at line %d, column %d\n",
            tokens[current_token - 1].line, tokens[current_token - 1].column);
        free_ast(for_node);
        return NULL;
    }

    // Parse condition
    ASTNode* condition = parse_expression();
    if (!condition) {
        fprintf(stderr, "Error: Expected condition in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, condition);

    // Match second ';'
    if (!match(TOKEN_SYMBOL, ";")) {
        fprintf(stderr, "Error: Expected ';' after condition in 'for' loop at line %d, column %d\n",
            tokens[current_token - 1].line, tokens[current_token - 1].column);
        free_ast(for_node);
        return NULL;
    }

    // Parse increment
    ASTNode* increment = parse_expression();
    if (!increment) {
        fprintf(stderr, "Error: Expected increment in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }
    add_child(for_node, increment);

    // Match ')'
    if (!match(TOKEN_SYMBOL, ")")) {
        fprintf(stderr, "Error: Expected ')' after increment in 'for' loop at line %d, column %d\n",
            tokens[current_token].line, tokens[current_token].column);
        free_ast(for_node);
        return NULL;
    }

    // Parse loop body
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
    // Parse the left-hand side (LHS)
    ASTNode* lhs = parse_factor();
    if (!lhs) return NULL;

    while (peek() && get_precedence(peek()) >= min_precedence) {
        // Parse the operator
        Token* op_token = advance();

        // Parse the right-hand side (RHS) with increased precedence
        ASTNode* rhs = parse_expression_with_precedence(get_precedence(op_token) + 1);
        if (!rhs) {
            fprintf(stderr, "Error: Invalid right-hand side in expression\n");
            free_ast(lhs);
            return NULL;
        }

        // Create a binary operation node
        ASTNode* binary_op = create_node(NODE_EXPRESSION, *op_token);
        binary_op->inferred_type = TYPE_INT; // Set default inferred type

        // Check operator to deduce the type
        if (strcmp(op_token->value, "+") == 0 || strcmp(op_token->value, "-") == 0) {
            if (lhs->inferred_type == TYPE_FLOAT || rhs->inferred_type == TYPE_FLOAT) {
                binary_op->inferred_type = TYPE_FLOAT; // Promote to float if either side is float
            }
        }
        else if (strcmp(op_token->value, "&&") == 0 || strcmp(op_token->value, "||") == 0) {
            binary_op->inferred_type = TYPE_BOOL; // Logical operators result in boolean
        }

        add_child(binary_op, lhs);
        add_child(binary_op, rhs);

        // Update LHS for next iteration
        lhs = binary_op;
    }

    return lhs;
}

// Updated parse_expression function
ASTNode* parse_expression() {
    return parse_expression_with_precedence(0);
}


ASTNode* parse_term() {
    return parse_factor();
}

static void parse_embedded_expressions(ASTNode* node) {
    const char* str = node->token.value; // Original string token value
    char buffer[128];
    int i = 0, j = 0;

    while (str[i] != '\0') {
        if (str[i] == '$' && str[i + 1] == '{') {
            i += 2; // Skip "${"

            // Extract the expression inside "${ }"
            while (str[i] != '}' && str[i] != '\0') {
                buffer[j++] = str[i++];
            }
            buffer[j] = '\0'; // Null-terminate the expression

            if (str[i] == '}') i++; // Skip closing '}'

            // Parse the extracted expression
            ASTNode* expr = parse_expression_with_precedence(0);
            add_child(node, expr); // Add it as a child of the interpolation node
        }
        else {
            i++;
        }
    }
}

// Helper function to handle grouped expressions
static ASTNode* parse_grouped_expression() {
    advance(); // Consume '('
    ASTNode* expr = parse_expression(); // Parse the inner expression
    if (!expr) {
        fprintf(stderr, "Error: Invalid expression after '('\n");
        return NULL;
    }
    if (!match(TOKEN_SYMBOL, ")")) { // Expect closing ')'
        fprintf(stderr, "Error: Missing ')' in grouped expression\n");
        free_ast(expr);
        return NULL;
    }
    return expr;
}

// Helper function to handle string literals
static ASTNode* parse_string_literal(Token* token) {
    if (strstr(token->value, "${")) { // Interpolation detected
        ASTNode* node = create_node(NODE_STRING_INTERPOLATION, *token);
        parse_embedded_expressions(node); // Extract and parse interpolated expressions
        advance(); // Move to the next token
        return node;
    }
    // Regular string literal
    advance();
    return create_node(NODE_LITERAL, *token);
}

// Helper function to handle literals and identifiers
static ASTNode* parse_literal_or_identifier(Token* token) {
    advance();
    return create_node(NODE_FACTOR, *token);
}

// Refactored parse_factor function
ASTNode* parse_factor() {
    Token* token = peek();
    if (!token) {
        fprintf(stderr, "Error: Unexpected end of input in factor\n");
        return NULL;
    }

    // Handle grouped expressions
    if (token->type == TOKEN_SYMBOL && strcmp(token->value, "(") == 0) {
        return parse_grouped_expression();
    }

    // Handle string literals and interpolation
    if (token->type == TOKEN_STRING) {
        return parse_string_literal(token);
    }

    // Handle literals and identifiers
    if (token->type == TOKEN_LITERAL || token->type == TOKEN_IDENTIFIER) {
        return parse_literal_or_identifier(token);
    }

    // Unexpected token
    fprintf(stderr, "Error: Unexpected token '%s' in factor\n", token->value);
    advance(); // Skip invalid token
    return NULL;
}
// Helper function to parse the condition of an 'if' statement
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

// Helper function to parse a block in an 'if' or 'else' statement
static ASTNode* parse_if_block(const char* block_name) {
    ASTNode* block = parse_block();
    if (!block) {
        fprintf(stderr, "Error: Invalid %s block in 'if' statement\n", block_name);
    }
    return block;
}

// Refactored parse_if_statement function
ASTNode* parse_if_statement() {
    // Create the 'if' node
    ASTNode* if_node = check_memory_allocation(
        create_node(NODE_IF, (Token) { TOKEN_KEYWORD, "if", 0, 0 }),
        "parse_if_statement"
    );

    // Parse the condition
    ASTNode* condition = parse_if_condition();
    if (!condition) {
        free_ast(if_node);
        return NULL;
    }
    add_child(if_node, condition);

    // Parse the 'if' block
    ASTNode* if_block = parse_if_block("if");
    if (!if_block) {
        free_ast(if_node);
        return NULL;
    }
    add_child(if_node, if_block);

    // Parse the optional 'else' block
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
// Helper function to match a required symbol with error reporting
static int expect_symbol(const char* symbol, const char* error_message) {
    if (!match(TOKEN_SYMBOL, symbol)) {
        fprintf(stderr, "%s\n", error_message);
        return 0;
    }
    return 1;
}

// Helper function to parse the expression inside the 'print' statement
static ASTNode* parse_print_expression() {
    ASTNode* expression = parse_expression();
    if (!expression) {
        fprintf(stderr, "Error: Invalid expression in 'print' statement\n");
    }
    return expression;
}

// Refactored parse_print_statement function
ASTNode* parse_print_statement() {
    // Create the 'print' node
    ASTNode* print_node = check_memory_allocation(
        create_node(NODE_PRINT_STATEMENT, tokens[current_token - 1]),
        "parse_print_statement"
    );

    // Expect '(' after 'print'
    if (!expect_symbol("(", "Error: Expected '(' after 'print'")) {
        free_ast(print_node);
        return NULL;
    }

    // Parse the expression inside the parentheses
    ASTNode* expression = parse_print_expression();
    if (!expression) {
        free_ast(print_node);
        return NULL;
    }
    add_child(print_node, expression);

    // Expect ')' after the expression
    if (!expect_symbol(")", "Error: Expected ')' after 'print' expression")) {
        free_ast(print_node);
        return NULL;
    }

    // Expect ';' after the statement
    if (!expect_symbol(";", "Error: Expected ';' after 'print' statement")) {
        free_ast(print_node);
        return NULL;
    }

    return print_node;
}
// Helper function to validate and retrieve the record name
static Token* parse_record_name(Token* record_token) {
    Token* name_token = advance();
    if (!name_token || name_token->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected record name after 'record' at line %d, column %d.\n",
            record_token->line, record_token->column);
        return NULL;
    }
    return name_token;
}

// Helper function to match and validate opening brace
static int validate_opening_brace(const Token* name_token) {
    if (!match(TOKEN_SYMBOL, "{")) {
        fprintf(stderr, "Error: Expected '{' after record name '%s' at line %d, column %d.\n",
            name_token->value, name_token->line, name_token->column);
        return 0;
    }
    return 1;
}

// Helper function to parse a single record field
static ASTNode* parse_record_field(const Token* name_token, const Token* record_token) {
    // Parse field name
    Token* field_name = advance();
    if (!field_name || field_name->type != TOKEN_IDENTIFIER) {
        fprintf(stderr, "Error: Expected field name in record '%s' at line %d, column %d.\n",
            name_token->value, record_token->line, record_token->column);
        return NULL;
    }

    // Check for '=' after the field name
    if (!match(TOKEN_OPERATOR, "=")) {
        fprintf(stderr, "Error: Expected '=' after field name '%s' in record '%s' at line %d, column %d.\n",
            field_name->value, name_token->value, field_name->line, field_name->column);
        return NULL;
    }

    // Parse field value
    Token* field_value = advance();
    if (!field_value || (field_value->type != TOKEN_LITERAL && field_value->type != TOKEN_IDENTIFIER)) {
        fprintf(stderr, "Error: Expected value for field '%s' in record '%s' at line %d, column %d.\n",
            field_name->value, name_token->value, field_name->line, field_name->column);
        return NULL;
    }

    // Create a field node
    ASTNode* field_node = create_node(NODE_VARIABLE_DECLARATION, *field_name);
    return field_node;
}

// Helper function to parse all fields in the record
static int parse_record_fields(ASTNode* record_node, const Token* name_token, const Token* record_token) {
    while (!match(TOKEN_SYMBOL, "}")) {
        if (!peek()) {
            fprintf(stderr, "Error: Unterminated record definition for '%s' starting at line %d, column %d.\n",
                name_token->value, record_token->line, record_token->column);
            return 0; // Failure
        }

        ASTNode* field_node = parse_record_field(name_token, record_token);
        if (!field_node) {
            return 0; // Failure
        }
        add_child(record_node, field_node);
    }
    return 1; // Success
}

// Refactored parse_record_definition function
ASTNode* parse_record_definition() {
    // Advance to the 'record' keyword
    Token* record_token = advance();
    if (!record_token || strcmp(record_token->value, "record") != 0) {
        fprintf(stderr, "Error: Expected 'record' keyword.\n");
        return NULL;
    }

    // Parse the record name
    Token* name_token = parse_record_name(record_token);
    if (!name_token) return NULL;

    // Validate the opening brace
    if (!validate_opening_brace(name_token)) return NULL;

    // Create the record AST node
    ASTNode* record_node = check_memory_allocation(create_node(NODE_STRUCT, *name_token), "parse_record_definition");

    // Parse the fields inside the record
    if (!parse_record_fields(record_node, name_token, record_token)) {
        free_ast(record_node);
        return NULL;
    }

    printf("Record '%s' successfully parsed with %d fields.\n", name_token->value, record_node->child_count);
    return record_node;
}
ASTNode* parse_switch_statement() {
    if (!match(TOKEN_KEYWORD, "switch")) return NULL;

    ASTNode* switch_node = create_node(NODE_SWITCH, (Token) { TOKEN_KEYWORD, "switch", 0, 0 });
    if (!match(TOKEN_SYMBOL, "(")) {
        fprintf(stderr, "Error: Expected '(' after 'switch'.\n");
        synchronize();
        return NULL;
    }

    ASTNode* condition = parse_expression(); // Parse switch condition
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

    while (peek() && !match(TOKEN_SYMBOL, "}")) {
        ASTNode* case_node = parse_case_statement();
        if (case_node) {
            add_child(switch_node, case_node);
        }
        else {
            fprintf(stderr, "Warning: Skipping invalid case in 'switch'.\n");
            advance(); // Skip the invalid token to recover
        }
    }

    return switch_node;
}

ASTNode* parse_case_statement() {
    if (match(TOKEN_KEYWORD, "case")) {
        ASTNode* case_node = create_node(NODE_CASE, (Token) { TOKEN_KEYWORD, "case", 0, 0 });
        ASTNode* case_value = parse_expression(); // Case value expression
        if (!case_value) {
            fprintf(stderr, "Error: Missing or invalid case value.\n");
            return NULL;
        }
        add_child(case_node, case_value);

        if (!match(TOKEN_SYMBOL, ":")) {
            fprintf(stderr, "Error: Expected ':' after 'case' value.\n");
            return NULL;
        }

        while (peek() && !match(TOKEN_KEYWORD, "case") && !match(TOKEN_KEYWORD, "default") && !match(TOKEN_SYMBOL, "}")) {
            ASTNode* statement = parse_statement();
            if (statement) add_child(case_node, statement);
        }

        return case_node;
    }
    else if (match(TOKEN_KEYWORD, "default")) {
        return parse_default_case();
    }

    return NULL;
}

ASTNode* parse_default_case() {
    if (!match(TOKEN_SYMBOL, ":")) {
        fprintf(stderr, "Error: Expected ':' after 'default'.\n");
        return NULL;
    }

    ASTNode* default_node = create_node(NODE_DEFAULT, (Token) { TOKEN_KEYWORD, "default", 0, 0 });

    while (peek() && !match(TOKEN_KEYWORD, "case") && !match(TOKEN_SYMBOL, "}")) {
        ASTNode* statement = parse_statement();
        if (statement) add_child(default_node, statement);
    }

    return default_node;
}
