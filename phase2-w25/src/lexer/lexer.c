/* lexer.c */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "../../include/tokens.h"
#include "../../include/lexer.h"

static int current_line = 1;
static int current_column = 1; // Column tracking
static char last_token_type = 'x';

// Keywords table
static struct {
    const char* word;
    TokenType type;
} keywords[] = {
    {"if", TOKEN_IF},
    {"while", TOKEN_WHILE},
    {"factorial", TOKEN_FACT},
    {"int", TOKEN_INT},
    {"print", TOKEN_PRINT}
};

static int is_keyword(const char* word) {
    for (int i = 0; i < sizeof(keywords) / sizeof(keywords[0]); i++) {
        if (strcmp(word, keywords[i].word) == 0) {
            return keywords[i].type;
        }
    }
    return 0;
}

void print_error(ErrorType error, int line, int column, const char* lexeme) {
    printf("Lexical Error at line %d, column %d: ", line, column);
    switch(error) {
        case ERROR_INVALID_CHAR:
            printf("Invalid character '%s'\n", lexeme);
            break;
        case ERROR_INVALID_NUMBER:
            printf("Invalid number format\n");
            break;
        case ERROR_CONSECUTIVE_OPERATORS:
            printf("Consecutive operators not allowed\n");
            break;
        case ERROR_INVALID_IDENTIFIER:
            printf("Invalid identifier\n");
            break;
        case ERROR_UNEXPECTED_TOKEN:
            printf("Unexpected token '%s'\n", lexeme);
            break;
        default:
            printf("Unknown error\n");
    }
}

void print_token(Token token) {
    if (token.error != ERROR_NONE) {
        print_error(token.error, token.line, token.column, token.lexeme);
        return;
    }

    printf("Token: ");
    switch(token.type) {
        case TOKEN_NUMBER:     printf("NUMBER"); break;
        case TOKEN_OPERATOR:   printf("OPERATOR"); break;
        case TOKEN_IDENTIFIER: printf("IDENTIFIER"); break;
        case TOKEN_EQUALS:     printf("EQUALS"); break;
        case TOKEN_COMPARE:    printf("COMPARE"); break;
        case TOKEN_SEMICOLON:  printf("SEMICOLON"); break;
        case TOKEN_LPAREN:     printf("LPAREN"); break;
        case TOKEN_RPAREN:     printf("RPAREN"); break;
        case TOKEN_LBRACE:     printf("LBRACE"); break;
        case TOKEN_RBRACE:     printf("RBRACE"); break;
        case TOKEN_IF:         printf("IF"); break;
        case TOKEN_WHILE:      printf("WHILE"); break;
        case TOKEN_FACT:       printf("FACTORIAL"); break;
        case TOKEN_INT:        printf("INT"); break;
        case TOKEN_PRINT:      printf("PRINT"); break;
        case TOKEN_EOF:        printf("EOF"); break;
        default:              printf("UNKNOWN");
    }
    printf(" | Lexeme: '%s' | Line: %d\n", token.lexeme, token.line);
}

Token get_next_token(const char* input, int* pos) {
    Token token = {TOKEN_ERROR, "", current_line, current_column, ERROR_NONE};
    char c;

    // Skip whitespace and track line numbers
    while ((c = input[*pos]) != '\0' && (c == ' ' || c == '\n' || c == '\t')) {
        if (c == '\n') {
            current_line++;
            current_column = 1; // Reset column on newline
        } 
        else {
            current_column++; // Add 1 to column for whitespace
        }
        (*pos)++;
    }

    if (input[*pos] == '\0') {
        token.type = TOKEN_EOF;
        strcpy(token.lexeme, "EOF");
        return token;
    }

    int token_column = current_column;

    c = input[*pos];

    // Handle numbers
    if (isdigit(c)) {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            current_column++; // Update column
            (*pos)++;
            c = input[*pos];
        } while (isdigit(c) && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';
        token.type = TOKEN_NUMBER;
        token.column = token_column; // Assign starting column
        return token;
    }

    // Handle identifiers and keywords
    if (isalpha(c) || c == '_') {
        int i = 0;
        do {
            token.lexeme[i++] = c;
            (*pos)++;
            current_column++; // Update column
            c = input[*pos];
        } while ((isalnum(c) || c == '_') && i < sizeof(token.lexeme) - 1);

        token.lexeme[i] = '\0';

        // Check if it's a keyword
        TokenType keyword_type = is_keyword(token.lexeme);
        if (keyword_type) {
            token.type = keyword_type;
        } else {
            token.type = TOKEN_IDENTIFIER;
        }
        token.column = token_column;
        return token;
    }

    // Handle operators and delimiters
    (*pos)++;
    token.column = token_column; // Assign starting column
    token.lexeme[0] = c;
    token.lexeme[1] = '\0';
    char d;

    switch(c) {
        case '+': case '-': case '*': case '/':
            if (last_token_type == 'o') {
                token.error = ERROR_CONSECUTIVE_OPERATORS;
                return token;
            }
            token.type = TOKEN_OPERATOR;
            last_token_type = 'o';
            break;
        case '=':
            d = input[*pos];
            if (d == '='){
                token.type = TOKEN_COMPARE;
                token.lexeme[1] = d;
                token.lexeme[2] = '\0';
                (*pos)++;
                current_column++; // Update column
                break;
            }
            token.type = TOKEN_EQUALS;
            break;
        case '<':
            token.type = TOKEN_COMPARE;
            d = input[*pos];
            if (d == '='){
                token.lexeme[1] = d;
                token.lexeme[2] = '\0';
                (*pos)++;
                current_column++; // Update column
            }
            break;
        case '>':
            token.type = TOKEN_COMPARE;
            d = input[*pos];
            if (d == '='){
                token.lexeme[1] = d;
                token.lexeme[2] = '\0';
                (*pos)++;
                current_column++; // Update column
            }
            break;
        case ';':
            token.type = TOKEN_SEMICOLON;
            break;
        case '(':
            token.type = TOKEN_LPAREN;
            break;
        case ')':
            token.type = TOKEN_RPAREN;
            break;
        case '{':
            token.type = TOKEN_LBRACE;
            break;
        case '}':
            token.type = TOKEN_RBRACE;
            break;
        default:
            token.error = ERROR_INVALID_CHAR;
            break;
    }

    return token;
}

// int main() {
//     const char *input = "int x = 123;\n"   // Basic declaration and number
//                        "test_var = 456;\n"  // Identifier and assignment
//                        "print x;\n"         // Keyword and identifier
//                        "if (y > 10) {\n"    // Keywords, identifiers, operators
//                        "    @#$ invalid\n"  // Error case
//                        "    x = ++2;\n"     // Consecutive operator error
//                        "}";
//
//     printf("Analyzing input:\n%s\n\n", input);
//     int position = 0;
//     Token token;
//
//     do {
//         token = get_next_token(input, &position);
//         print_token(token);
//     } while (token.type != TOKEN_EOF);
//
//     return 0;
// }
