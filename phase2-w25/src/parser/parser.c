/* parser.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/parser.h"
#include "../../include/lexer.h"
#include "../../include/tokens.h"

// TODO 1: Add more parsing function declarations for:
// - if statements: if (condition) { ... }
// - while loops: while (condition) { ... }
// - repeat-until: repeat { ... } until (condition)
// - print statements: print x;
// - blocks: { statement1; statement2; }
// - factorial function: factorial(x)

// Current token being processed
static Token current_token;
static int position = 0;
static const char *source;

static void parse_error(ParseError error, Token token)
{
    // If your Token structure has a column field, you can enable the line below.
    // printf("Parse Error at line %d, column %d: ", token.line, token.column);
    printf("Parse Error at line %d: ", token.line);
    switch (error)
    {
    case PARSE_ERROR_UNEXPECTED_TOKEN:
        printf("Unexpected token '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_SEMICOLON:
        printf("Missing semicolon after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_IDENTIFIER:
        printf("Expected identifier after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_EQUALS:
        printf("Expected '=' after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_INVALID_EXPRESSION:
        printf("Invalid expression after '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_PARENTHESIS:
        printf("Expected parenthesis for line ended '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_BAD_PARENTHESIS:
        printf("Expected alternative parenthesis for line ended '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_CONDITION:
        printf("Missing condition near '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_MISSING_BLOCK:
        printf("Missing block braces near '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_INVALID_OPERATOR:
        printf("Invalid operator '%s'\n", token.lexeme);
        break;
    case PARSE_ERROR_FUNCTION_CALL_ERROR:
        printf("Function call error near '%s'\n", token.lexeme);
        break;
    default:
        printf("Unknown error\n");
    }
}

// Get next token
static void advance(void)
{
    current_token = get_next_token(source, &position);
}

// Create a new AST node
static ASTNode *create_node(ASTNodeType type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    if (node)
    {
        node->type = type;
        node->token = current_token;
        node->left = NULL;
        node->right = NULL;
    }
    return node;
}

// Match current token with expected type
static int match(TokenType type)
{
    return current_token.type == type;
}

// Expect a token type or error
static void expect(TokenType type)
{
    if (match(type))
    {
        advance();
    }
    else
    {
        parse_error(PARSE_ERROR_UNEXPECTED_TOKEN, current_token);
        exit(1); // Or implement error recovery
    }
}

// Forward declarations
static ASTNode *parse_statement(void);

// TODO 3: Add parsing functions for each new statement type
// static ASTNode* parse_if_statement(void) { ... }
// static ASTNode* parse_while_statement(void) { ... }
// static ASTNode* parse_repeat_statement(void) { ... }
// static ASTNode* parse_print_statement(void) { ... }
// static ASTNode* parse_block(void) { ... }
// static ASTNode* parse_factorial(void) { ... }

static ASTNode *parse_expression(void);
static ASTNode *parse_expr_prec(int min_prec);


// Parse if statement: if (x) {y}
// UNTESTED
static ASTNode *parse_if_statement(void)
{

    // the 'if' itself
    ASTNode *node = create_node(AST_IF);
    advance();

    // '('
    if (!match(TOKEN_LPAREN))
    {
        if (match(TOKEN_LBRACE))
        {
            parse_error(PARSE_ERROR_BAD_PARENTHESIS, current_token);
            exit(1);
        }
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }

    // Conditional expression (consumes '(' now)
    node->left = parse_expression();

    // ')'
    if (!match(TOKEN_RPAREN))
    {
        if (match(TOKEN_RBRACE))
        {
            parse_error(PARSE_ERROR_BAD_PARENTHESIS, current_token);
            exit(1);
        }
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }
    
    advance();

    // '{'
    if (!match(TOKEN_LBRACE))
    {
        if (match(TOKEN_LPAREN))
        {
            parse_error(PARSE_ERROR_BAD_PARENTHESIS, current_token);
            exit(1);
        }
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }

    advance();

    // Could be one or multiple statements; copied parse_program logic
    ASTNode *then = create_node(AST_PROGRAM);
    ASTNode *curr = then;
    while (!match(TOKEN_RBRACE) && !match(TOKEN_EOF)) // must end at '}' or EOF
    {
        curr->left = parse_statement();
        if (!match(TOKEN_RBRACE) && !match(TOKEN_EOF))
        {
            curr->right = create_node(AST_PROGRAM);
            curr = curr->right;
        }
    }
    
    node->right = then;
    
    // '}' (Will have advanced to '}' or EOF by now)
    if (!match(TOKEN_RBRACE))
    {
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse while statement: while (x) {y}
// UNTESTED
static ASTNode *parse_while_statement(void)
{

    // the 'while' itself
    ASTNode *node = create_node(AST_WHILE);
    advance();

    // '('
    if (!match(TOKEN_LPAREN))
    {
        if (match(TOKEN_LBRACE))
        {
            parse_error(PARSE_ERROR_BAD_PARENTHESIS, current_token);
            exit(1);
        }
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }

    // Conditional expression (consumes '(' now)
    node->left = parse_expression();

    // ')'
    if (!match(TOKEN_RPAREN))
    {
        if (match(TOKEN_RBRACE))
        {
            parse_error(PARSE_ERROR_BAD_PARENTHESIS, current_token);
            exit(1);
        }
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }
    
    advance();

    // '{'
    if (!match(TOKEN_LBRACE))
    {
        if (match(TOKEN_LPAREN))
        {
            parse_error(PARSE_ERROR_BAD_PARENTHESIS, current_token);
            exit(1);
        }
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }

    advance();

    // Could be one or multiple statements; copied parse_program logic
    ASTNode *doing = create_node(AST_PROGRAM);
    ASTNode *curr = doing;
    while (!match(TOKEN_RBRACE) && !match(TOKEN_EOF)) // must end at '}' or EOF
    {
        curr->left = parse_statement();
        if (!match(TOKEN_RBRACE) && !match(TOKEN_EOF))
        {
            curr->right = create_node(AST_PROGRAM);
            curr = curr->right;
        }
    }
    
    node->right = doing;
    
    // '}' (Will have advanced to '}' or EOF by now)
    if (!match(TOKEN_RBRACE))
    {
        parse_error(PARSE_ERROR_MISSING_PARENTHESIS, current_token);
        exit(1);
    }
    
    advance();
    return node;
}

// Parse variable declaration: int x;
static ASTNode *parse_declaration(void)
{
    ASTNode *node = create_node(AST_VARDECL);
    advance(); // consume 'int'

    if (!match(TOKEN_IDENTIFIER))
    {
        parse_error(PARSE_ERROR_MISSING_IDENTIFIER, current_token);
        exit(1);
    }

    node->token = current_token;
    advance();

    if (!match(TOKEN_SEMICOLON))
    {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse assignment: x = 5;
static ASTNode *parse_assignment(void)
{
    ASTNode *node = create_node(AST_ASSIGN);
    node->left = create_node(AST_IDENTIFIER);
    node->left->token = current_token;
    advance();

    if (!match(TOKEN_EQUALS))
    {
        parse_error(PARSE_ERROR_MISSING_EQUALS, current_token);
        exit(1);
    }
    advance();

    node->right = parse_expression();

    if (!match(TOKEN_SEMICOLON))
    {
        parse_error(PARSE_ERROR_MISSING_SEMICOLON, current_token);
        exit(1);
    }
    advance();
    return node;
}

// Parse statement
static ASTNode *parse_statement(void)
{
    if (match(TOKEN_INT))
    {
        return parse_declaration();
    }
    else if (match(TOKEN_IDENTIFIER))
    {
        return parse_assignment();
    }
    else if (match(TOKEN_IF)) 
    {
        return parse_if_statement();
    }
    else if (match(TOKEN_WHILE)) 
    {
        return parse_while_statement();
    }
    // TODO 4: Add cases for new statement types
    // else if (match(TOKEN_REPEAT)) return parse_repeat_statement();
    // else if (match(TOKEN_PRINT)) return parse_print_statement();
    // ...

    printf("Syntax Error: Unexpected token\n");
    exit(1);
}

// Parse expression (currently only handles numbers and identifiers)

// TODO 5: Implement expression parsing
// Current expression parsing is basic. Need to implement:
// - Binary operations (+-*/)
// - Comparison operators (<, >, ==, etc.)
// - Operator precedence
// - Parentheses grouping
// - Function calls

static ASTNode *parse_expression(void)
{
    ASTNode *node;

    if (match(TOKEN_LPAREN)) {
        advance();
        node = parse_expr_prec(0);
        if (!match(TOKEN_RPAREN)) {
            printf("Syntax Error: Expected ')'\n");
            exit(1);
        }
        advance();
    }
    else if (match(TOKEN_NUMBER))
    {
        node = create_node(AST_NUMBER);
        advance();
    }
    else if (match(TOKEN_IDENTIFIER))
    {
        node = create_node(AST_IDENTIFIER);
        advance();
    }
    else
    {
        printf("Syntax Error: Expected expression\n");
        exit(1);
    }

    return node;
}

static int get_precedence(Token token)
{
    if (token.type != TOKEN_OPERATOR)
        return -1;
    if (strcmp(token.lexeme, "==") == 0 || strcmp(token.lexeme, "!=") == 0 ||
        strcmp(token.lexeme, "<") == 0 || strcmp(token.lexeme, ">") == 0 ||
        strcmp(token.lexeme, "<=") == 0 || strcmp(token.lexeme, ">=") == 0)
        return 1;
    if (strcmp(token.lexeme, "+") == 0 || strcmp(token.lexeme, "-") == 0)
        return 2;
    if (strcmp(token.lexeme, "*") == 0 || strcmp(token.lexeme, "/") == 0)
        return 3;
    return -1;
}

static ASTNode *parse_expr_prec(int min_prec)
{
    
    ASTNode *left = parse_expression();

    while (match(TOKEN_OPERATOR))
    {
        int prec = get_precedence(current_token);
        if (prec < min_prec)
            break;

        Token op = current_token;
        advance();

        ASTNode *right = parse_expr_prec(prec + 1);

        ASTNode *binop_node = create_node(AST_BINOP);
        binop_node->token = op;
        binop_node->left = left;
        binop_node->right = right;

        left = binop_node;
    }

    return left;
}

// Parse program (multiple statements)
static ASTNode *parse_program(void)
{
    ASTNode *program = create_node(AST_PROGRAM);
    ASTNode *current = program;

    while (!match(TOKEN_EOF))
    {
        current->left = parse_statement();
        if (!match(TOKEN_EOF))
        {
            current->right = create_node(AST_PROGRAM);
            current = current->right;
        }
    }

    return program;
}

// Initialize parser
void parser_init(const char *input)
{
    source = input;
    position = 0;
    advance(); // Get first token
}

// Main parse function
ASTNode *parse(void)
{
    return parse_program();
}

// Print AST (for debugging)
void print_ast(ASTNode *node, int level)
{
    if (!node)
        return;

    // Indent based on level
    for (int i = 0; i < level; i++)
        printf("  ");

    // Print node info
    switch (node->type)
    {
    case AST_PROGRAM:
        printf("Program\n");
        break;
    case AST_VARDECL:
        printf("VarDecl: %s\n", node->token.lexeme);
        break;
    case AST_ASSIGN:
        printf("Assign\n");
        break;
    case AST_NUMBER:
        printf("Number: %s\n", node->token.lexeme);
        break;
    case AST_IDENTIFIER:
        printf("Identifier: %s\n", node->token.lexeme);
        break;

    // TODO 6: Add cases for new node types
    // case AST_IF: printf("If\n"); break;
    // case AST_WHILE: printf("While\n"); break;
    // case AST_REPEAT: printf("Repeat-Until\n"); break;
    // case AST_BLOCK: printf("Block\n"); break;
    case AST_BINOP:
        printf("BinaryOp: %s\n", node->token.lexeme);
        break;
    default:
        printf("Unknown node type\n");
    }

    // Print children
    print_ast(node->left, level + 1);
    print_ast(node->right, level + 1);
}

// Free AST memory
void free_ast(ASTNode *node)
{
    if (!node)
        return;
    free_ast(node->left);
    free_ast(node->right);
    free(node);
}

// Main function for testing
int main()
{
    // Test with both valid and invalid inputs
    const char *input = "int x;\n"   // Valid declaration
                        "x = 42;\n"; // Valid assignment;
    // TODO 8: Add more test cases and read from a file:
    const char *invalid_input = "int x;\n"
                                "x = 42;\n"
                                "int ;";

    printf("Parsing input:\n%s\n", invalid_input);
    parser_init(invalid_input);
    ASTNode *ast = parse();

    printf("\nAbstract Syntax Tree:\n");
    print_ast(ast, 0);

    free_ast(ast);
    return 0;
}
