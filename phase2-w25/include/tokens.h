/* tokens.h */
#ifndef TOKENS_H
#define TOKENS_H

typedef enum {
    TOKEN_EOF,
    TOKEN_NUMBER,      // e.g., "123", "456"
    TOKEN_OPERATOR,    // +, -, *, /
    TOKEN_IDENTIFIER,  // Variable names
    TOKEN_EQUALS,      // =
    TOKEN_SEMICOLON,   // ;
    TOKEN_LPAREN,      // (
    TOKEN_RPAREN,      // )
    TOKEN_LBRACE,      // {
    TOKEN_RBRACE,      // }
    TOKEN_IF,          // if keyword
    TOKEN_WHILE,       // while keyword
    TOKEN_INT,         // int keyword
    TOKEN_PRINT,       // print keyword
    TOKEN_ERROR
} TokenType;

typedef enum {
    ERROR_NONE,
    ERROR_INVALID_CHAR,
    ERROR_INVALID_NUMBER,
    ERROR_CONSECUTIVE_OPERATORS,
    ERROR_INVALID_IDENTIFIER,
    ERROR_UNEXPECTED_TOKEN
} ErrorType;

typedef struct {
    TokenType type;
    char lexeme[100];   // Actual text of the token
    int line;           // Line number in source file
    ErrorType error;    // Error type if any
} Token;

#endif /* TOKENS_H */
