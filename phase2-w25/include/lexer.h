/* lexer.h */
#ifndef LEXER_H
#define LEXER_H

#include "tokens.h"

// Lexer functions that need to be visible to other files
Token get_next_token(const char* input, int* pos);
void print_token(Token token);
void print_error(ErrorType error, int line, int column, const char* lexeme);

#endif /* LEXER_H */