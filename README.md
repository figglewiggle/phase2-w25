# Phase 3: Semantic Analysis - Implementation Guide

## Overview

In this phase, you will build upon **your lexical analyzer and parser** to create a semantic analyzer that performs contextual checks on the Abstract Syntax Tree (AST). Semantic analysis ensures that the program is not just syntactically correct, but semantically meaningful in accordance to the language rules **you have defined**. This time, no template code will be provided (as we want you to build on your own lexer and parser), however, the documentation will provide you with the guidance and conceptual starter kit to start working on your semantic analyzer.

## Understanding Semantic Analysis

### What is Semantic Analysis?

Semantic analysis is the compiler phase that:
- Checks if the constructs in the program are semantically valid (e.g., ensuring that a variable is declared before it's used: x = 5; would be invalid if x was never declared)
- Enforces language rules beyond syntax (type checking, scope rules, etc.)
- Detects logical errors that can't be caught by the parser (like type mismatches in assignments, accessing variables outside their scope, or using uninitialized variables)
- Builds a symbol table to track variables, their types, and scopes

### Why is Semantic Analysis Important?

While a parser ensures code follows the language grammar, it can't verify logical correctness. For example, a parser can verify that `x = y;` follows correct syntax, but it can't determine:
- Whether `x` and `y` have been declared
- If the types of `x` and `y` are compatible
- If `x` and `y` are being used within their scope

## Getting Started! 

### Project Structure

```
phase3/
├── include/
│   ├── tokens.h        # Token definitions from Phase 1
│   ├── lexer.h         # Lexer interface
│   ├── parser.h        # Parser definitions from Phase 2
│   └── semantic.h      # Semantic analyzer definitions
├── src/
│   ├── lexer/
│   │   └── lexer.c     # Lexer implementation from Phase 1
│   ├── parser/
│   │   └── parser.c    # Parser implementation from Phase 2
│   └── semantic/
│       └── semantic.c  # Semantic analyzer implementation
└── test/
    ├── input_valid.txt
    ├── input_invalid.txt
    └── input_semantic_error.txt
```

### Current Implementation

Your existing implementation includes:
- Lexical analyzer to generate tokens
- Parser to create an AST
- Error reporting framework

## Semantic Analyzer Implementation Guide

### Step 1: Define the Symbol Table Structure

The symbol table is the core data structure for tracking variables and their properties.
This table acts as a dictionary that stores information about each identifier (variable, function, etc.) in your program. For each symbol, it maintains essential information like:
- name of the identifier
- type information
- scope information (where the variable is accessible)
- initilization status

```c
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
```

### Step 2: Implement Symbol Table Operations! This is very important

Create operations for managing the symbol table:

```c
// Initialize a new symbol table
// Creates an empty symbol table structure with scope level set to 0
SymbolTable* init_symbol_table();

// Add a symbol to the table
// Inserts a new variable with given name, type, and line number into the current scope
void add_symbol(SymbolTable* table, const char* name, int type, int line);

// Look up a symbol in the table
// Searches for a variable by name across all accessible scopes
// Returns the symbol if found, NULL otherwise
Symbol* lookup_symbol(SymbolTable* table, const char* name);

// Enter a new scope level
// Increments the current scope level when entering a block (e.g., if, while)
void enter_scope(SymbolTable* table);

// Exit the current scope
// Decrements the current scope level when leaving a block
// Optionally removes symbols that are no longer in scope
void exit_scope(SymbolTable* table);

// Remove symbols from the current scope
// Cleans up symbols that are no longer accessible after leaving a scope
void remove_symbols_in_current_scope(SymbolTable* table);

// Free the symbol table memory
// Releases all allocated memory when the symbol table is no longer needed
void free_symbol_table(SymbolTable* table);
```

### Step 3: Implement Semantic Analysis Functions

The main semantic checking functions:

```c
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
```

### Step 4: Semantic Error Reporting

Define semantic error types and reporting functions:

```c
typedef enum {
    SEM_ERROR_NONE,
    SEM_ERROR_UNDECLARED_VARIABLE,
    SEM_ERROR_REDECLARED_VARIABLE,
    SEM_ERROR_TYPE_MISMATCH,
    SEM_ERROR_UNINITIALIZED_VARIABLE,
    SEM_ERROR_INVALID_OPERATION,
    SEM_ERROR_SEMANTIC_ERROR  // Generic semantic error
} SemanticErrorType;

// Report semantic errors
void semantic_error(SemanticErrorType error, const char* name, int line);
```

## Tasks to Complete

### 1. Symbol Table Management
- [ ] Implement the symbol table data structure
- [ ] Create functions to add, look up, and remove symbols
- [ ] Manage scopes (enter/exit)
- [ ] Track variable initialization status

### 2. Variable Checking
- [ ] Check variable declarations (no redeclaration in same scope)
- [ ] Check variable usage (must be declared before use)
- [ ] Track initialization status (warn if used uninitialized)

### 3. Type Checking
- [ ] Ensure operands have compatible types
- [ ] Validate operations based on operand types
- [ ] Add implicit type conversions where appropriate

### 4. Scope Analysis
- [ ] Implement block scoping rules
- [ ] Track variables across nested scopes
- [ ] Handle variable shadowing correctly

### 5. Flow Control Validation
- [ ] Validate conditions in if/while statements
- [ ] Check that control statements have valid bodies

### 6. Special Feature Validation
- [ ] Validate factorial function calls
- [ ] Ensure special operations have correct argument types

## Detailed Implementation Strategies

### Symbol Table Implementation

```c
// Initialize symbol table
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
```

### Semantic Analysis Functions

```c
// Analyze AST semantically
int analyze_semantics(ASTNode* ast) {
    SymbolTable* table = init_symbol_table();
    int result = check_program(ast, table);
    free_symbol_table(table);
    return result;
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
```

### Error Reporting

```c
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
```

## Sample Test Cases

### Valid Program
```c
int x;     // Declaration
int y;     // Another declaration
x = 5;     // Initialization
y = x + 2; // Using initialized variable
if (x > 0) {
    int z;     // Declaration in nested scope
    z = x + y; // Using outer scope variables
    print z;
}
```

### Invalid Programs

```c
// Undeclared variable
x = 5; // Error: x not declared

// Redeclaration in same scope
int x;
int x; // Error: x redeclared

// Uninitialized variable
int x;
int y;
y = x + 5; // Warning: x used before initialization

// Invalid operation
int x;
x = 5 / 0; // Error: division by zero

// Scope errors
if (x > 0) {
    int y;
}
y = 5; // Error: y not accessible in this scope
```

## Testing Your Semantic Analyzer

```c
int main() {
    const char* input = "int x;\n"
                        "x = 42;\n"
                        "if (x > 0) {\n"
                        "    int y;\n"
                        "    y = x + 10;\n"
                        "    print y;\n"
                        "}\n";
    
    printf("Analyzing input:\n%s\n\n", input);
    
    // Lexical analysis and parsing
    parser_init(input);
    ASTNode* ast = parse();
    
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
```

### Expected Output from a Semantic Analyzer

A well-implemented semantic analyzer should produce:

#### 1. For Valid Programs:
```
Analyzing input:
int x;
x = 42;
if (x > 0) {
    int y;
    y = x + 10;
    print y;
}

AST created. Performing semantic analysis...

Symbol Table Contents:
Name: y, Type: int, Scope: 1, Line: 4, Initialized: Yes
Name: x, Type: int, Scope: 0, Line: 1, Initialized: Yes

Semantic analysis successful. No errors found.
```

#### 2. For Programs with Semantic Errors:
```
Analyzing input:
x = 42;
if (x > 0) {
    int y;
    y = z + 10;
    print y;
}

AST created. Performing semantic analysis...

Semantic Error at line 1: Undeclared variable 'x'
Semantic Error at line 4: Undeclared variable 'z'

Semantic analysis failed. Errors detected.
```

#### 3. Symbol Table Dump Example:
```
== SYMBOL TABLE DUMP ==
Total symbols: 2

Symbol[0]:
  Name: x
  Type: int
  Scope Level: 0
  Line Declared: 1
  Initialized: Yes

Symbol[1]:
  Name: y
  Type: int
  Scope Level: 1
  Line Declared: 4
  Initialized: Yes
===================
```

## Common Challenges

1. **Handling nested scopes**
   - Keep track of scope levels
   - Implement scope entry/exit correctly
   - Remember that inner scopes can access outer scope variables!!!

2. **Type checking expressions**
   - Recursively check types in complex expressions
   - Implement type compatibility rules
   - Ensure type consistency across operations

3. **Managing memory**
   - Do not forget to clean up the symbol table properly
   - FREE all allocated symbols when done

4. **Managing initialization status**
   - Track whether variables have been assigned values
   - Warn about potentially uninitialized variables
   - Update initialization status across assignments

## Tips for Success

- **Begin with a simple implementation** that handles basic variable checks
- **Test incrementally** with small programs before tackling complex ones
- **Add features one at a time**, testing thoroughly after each addition
- **Create helper functions** for common semantic checks
- **Use consistent naming conventions** for your semantic functions
- **Document your symbol table design** and semantic rules

## Submission Requirements

1. Complete implementation of your `semantic.c` and `semantic.h`
2. Test cases demonstrating your semantic analyzer's capabilities
3. Documentation of your:
   - Symbol table implementation
   - Semantic checking rules
   - Error handling approach
   - Any extensions or enhancements

Good luck with your semantic analyzer implementation! 🚀