#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"
#include "../../include/semantic.h"


void semantic_error(SemanticErrorType error, const char* name, int line) {
    printf("Semantic Error at line %d: ", line);
    
    switch (error) {
        case SEM_ERROR_UNDECLARED_VARIABLE:
            printf("Undeclared variable '%s'\n", name);
            break;
        case SEM_ERROR_REDECLARED_VARIABLE:
            printf("Variable '%s' already declared in this scope\n", name);
            break;
        case SEM_ERROR_TYPE_MISMATCH:
            printf("Type mismatch involving '%s'\n", name);
            break;
        case SEM_ERROR_UNINITIALIZED_VARIABLE:
            printf("Variable '%s' may be used uninitialized\n", name);
            break;
        case SEM_ERROR_INVALID_OPERATION:
            printf("Invalid operation involving '%s'\n", name);
            break;
        default:
            printf("Unknown semantic error with '%s'\n", name);
    }
}

// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable* init_symbol_table() {
    SymbolTable* table = malloc(sizeof(SymbolTable));
    if (table) {
        table->head = NULL;
        table->current_scope = 0;
    }
    return table;
}

// Add symbol to table
void add_symbol(SymbolTable* table, const char* name, int type, int line) {
    Symbol* symbol = malloc(sizeof(Symbol));
    if (symbol) {
        strcpy(symbol->name, name);
        symbol->type = type;
        symbol->scope_level = table->current_scope;
        symbol->line_declared = line;
        symbol->is_initialized = 0;

        // Add to beginning of list
        symbol->next = table->head;
        table->head = symbol;
    }
}

// Look up symbol by name
Symbol* lookup_symbol(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Look up symbol in current scope only
Symbol* lookup_symbol_current_scope(SymbolTable* table, const char* name) {
    Symbol* current = table->head;
    while (current) {
        if (strcmp(current->name, name) == 0 &&
            current->scope_level == table->current_scope) {
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// Enter a new scope level
// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable* table){
    table->current_scope = (table->current_scope + 1);
}

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable* table){
    Symbol *prev;
    Symbol *curr;
    Symbol *next;

    // Are there any symbols?
    if (!(table->head)){
        return;
    }

    // Special case: table head is being removed
    curr = table->head;
    while (curr->scope_level == table->current_scope){
        next = curr->next;
        free(curr);
        curr = next;
        table->head = curr;
    }

    // Are there any symbols left?
    if (!(table->head)){
        return;
    }

    // Regular case
    curr = table->head; // Known to be in a broader scope
    while (curr->next){ // Only triggers if |table| > 1

        // Remove symbol of current scope and adjust links
        if (curr->scope_level == table->current_scope){
            next = curr->next;
            free(curr);
            curr = next;
            prev->next = curr;
        }

        // Shift further down the list
        // Will always happen during the first loop
        else {
            prev = curr;
            curr = curr->next;
        }
    }

    // Special case: the last symbol is being removed
    if (curr->scope_level == table->current_scope){
        prev->next=NULL;
        free(curr);
    }
}


// Exit the current scope
// Decrements the current scope level when leaving a block
// Optionally removes symbols that are no longer in scope
void exit_scope(SymbolTable* table){
    remove_symbols_in_current_scope(table);
    table->current_scope = (table->current_scope) - 1;
}



// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable* table){
    Symbol *curr = table->head;
    Symbol *next;

    // Clear the table's symbols
    while (curr){
        next = curr->next;
        free(curr);
        curr = next;
    }

    // Clear the table itself
    free(table);
}


int check_statement(ASTNode* node, SymbolTable* table) {
    // Null nodes are valid
    if (node == NULL) {
        return 1; // Empty node is valid
    }

    switch (node->type) {
        case AST_VARDECL:
            return check_declaration(node, table);
        case AST_ASSIGN:
            return check_assignment(node, table);
        case AST_PRINT:
            return check_expression(node->left, table);
        case AST_IF:
            // Check condition and branch
                return check_condition(node->left, table) && check_statement(node->right, table);
        case AST_WHILE:
            // Check condition and branch
                return check_condition(node->left, table) && check_statement(node->right, table);
        case AST_BLOCK:
            return check_block(node, table);
        case AST_REPEAT:
            // Check statement and condition
                return check_statement(node->left, table) && check_condition(node->right, table);
        default:
            semantic_error(SEM_ERROR_INVALID_OPERATION, node->token.lexeme, node->token.line);
        return 0; // Unknown statement type
    }
}

// Check program node
int check_program(ASTNode* node, SymbolTable* table) {
    if (!node) return 1;
    
    int result = 1;
    
    if (node->type == AST_PROGRAM) {
        // Check left child (statement)
        if (node->left) {
            result = check_statement(node->left, table) && result;
        }
        
        // Check right child (rest of program)
        if (node->right) {
            result = check_program(node->right, table) && result;
        }
    }
    
    return result;
}

// Analyze AST semantically
int analyze_semantics(ASTNode* ast) {
    SymbolTable* table = init_symbol_table();
    int result = check_program(ast, table);
    free_symbol_table(table);
    return result;
}


// Check declaration node
int check_declaration(ASTNode* node, SymbolTable* table) {
    if (node->type != AST_VARDECL) {
        return 0;
    }

    const char* name = node->token.lexeme;

    // Check if variable already declared in current scope
    Symbol* existing = lookup_symbol_current_scope(table, name);
    if (existing) {
        semantic_error(SEM_ERROR_REDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Add to symbol table
    add_symbol(table, name, TOKEN_INT, node->token.line);
    return 1;
}


// Check an expression for type correctness
int check_expression(ASTNode* node, SymbolTable* table){
    // empty node is invalid expression
    if (node == NULL) {
        return 0; 
    }

    switch (node->type) {
        // Numbers are valid expressions
        case AST_NUMBER:
            return TOKEN_INT;
        // Identifiers are valid expressions if they are declared
        case AST_IDENTIFIER: {
            // Check if variable has already been declared
            Symbol* symbol = lookup_symbol(table, node->token.lexeme);
            if (!symbol) {
                semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, node->token.lexeme, node->token.line);
                return 0;
            }
            // Check if variable has not been previously initialized
            else if (!symbol->is_initialized) {
                semantic_error(SEM_ERROR_UNINITIALIZED_VARIABLE, node->token.lexeme, node->token.line);
                return 0; 
            }
            else {
                // Return the type of the expression
                return symbol->type;
            }
        }
        case AST_BINOP:
            // recursively check left and right expressions
            int left_valid = check_expression(node->left, table);
            int right_valid = check_expression(node->right, table);
            // Check if left and right side of the binary operation are valid
            if (left_valid == 0 || right_valid == 0) {
                return 0; 
            } 
            if (left_valid != right_valid) {
                semantic_error(SEM_ERROR_TYPE_MISMATCH, node->token.lexeme, node->token.line);
                return 0; 
            }
            // Return the type of the expression
            return left_valid; 
        default:
            semantic_error(SEM_ERROR_INVALID_OPERATION, node->token.lexeme, node->token.line);
            return 0;
    }
}

// Check assignment node
int check_assignment(ASTNode* node, SymbolTable* table) {
    if (node->type != AST_ASSIGN || !node->left || !node->right) {
        return 0;
    }

    const char* name = node->left->token.lexeme;

    // Check if variable exists
    Symbol* symbol = lookup_symbol(table, name);
    if (!symbol) {
        semantic_error(SEM_ERROR_UNDECLARED_VARIABLE, name, node->token.line);
        return 0;
    }

    // Check expression
    int expr_valid = check_expression(node->right, table);

    // Mark as initialized
    if (expr_valid) {
        symbol->is_initialized = 1;
    }

    return expr_valid;
}

// Check a condition (e.g., in if statements)
int check_condition(ASTNode* node, SymbolTable* table){
    // Null node is invalid
    if (node == NULL) {
        return 0;
    }

    // Check the condition expression
    int result = check_expression(node, table);

    // s if the expression is valid
    if (result == 0) {
        return 0;
    }

    // conditions must be an integer
    if (result != TOKEN_INT) {
        semantic_error(SEM_ERROR_TYPE_MISMATCH, node->token.lexeme, node->token.line);
        return 0;
    }

    return 1;
}

// Check a block of statements, handling scope
int check_block(ASTNode* node, SymbolTable* table){
    if (node->type != AST_BLOCK) {
        return 0;
    }

    // Enter new scope
    enter_scope(table);

    // Left side: check the first statement in the block
    // Right side: check the rest of the block
    int result = check_statement(node->left, table) && check_block(node->right, table);

    // Exit scope
    exit_scope(table);

    return result;
}


int main() {
    const char* input = "int x;\n"
                        "x = 42;\n"
                        "if (x > 0) {\n"
                        "    int y;\n"
                        "    y = x + 10;\n"
                        "    print y;\n"
                        "}\n";

    parser_init(input);
    ASTNode *ast = parse();
    //
    // printf("\nAbstract Syntax Tree:\n");
    // print_ast(ast, 0);

    printf("Analyzing input:\n%s\n\n", input);

    // Lexical analysis and parsing

    printf("AST created. Performing semantic analysis...\n\n");

    // Semantic analysis
    int result = analyze_semantics(ast);

    if (result) {
        printf("Semantic analysis successful. No errors found.\n");
    } else {
        printf("Semantic analysis failed. Errors detected.\n");
    }

    // Clean up
    free_ast(ast);

    return 0;
}
