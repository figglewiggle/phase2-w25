#ifndef SEMANTIC_H
#define SEMANTIC_H

// Basic symbol structure
typedef struct Symbol {
    char name[100];          // Variable name
    int type;                // Data type (int, etc.)
    int scope_level;         // Scope nesting level
    int line_declared;       // Line where declared
    int is_initialized;      // Has been assigned a value?
    struct Symbol* next;     // For linked list implementation
} Symbol;

// Symbol table
typedef struct {
    Symbol* head;            // First symbol in the table
    int current_scope;       // Current scope level
} SymbolTable;

// Semantic errors
typedef enum {
    SEM_ERROR_NONE,
    SEM_ERROR_UNDECLARED_VARIABLE,
    SEM_ERROR_REDECLARED_VARIABLE,
    SEM_ERROR_TYPE_MISMATCH,
    SEM_ERROR_UNINITIALIZED_VARIABLE,
    SEM_ERROR_INVALID_OPERATION,
    SEM_ERROR_SEMANTIC_ERROR  // Generic semantic error
} SemanticErrorType;

// Main semantic analysis function
int analyze_semantics(ASTNode* ast);

// Check a variable declaration
int check_declaration(ASTNode* node, SymbolTable* table);

// Check a variable assignment
int check_assignment(ASTNode* node, SymbolTable* table);

// Check an expression for type correctness
int check_expression(ASTNode* node, SymbolTable* table);

// Check a block of statements, handling scope
int check_block(ASTNode* node, SymbolTable* table);

// Check a condition (e.g., in if statements)
int check_condition(ASTNode* node, SymbolTable* table);

// Report semantic errors
void semantic_error(SemanticErrorType error, const char* name, int line);

#endif /* SEMANTIC_H */
