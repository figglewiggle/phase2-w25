## Error Codes

### Lexical Errors
- **`ERROR_INVALID_CHAR`**  
  Raised when an invalid character is encountered in the input.

- **`ERROR_INVALID_NUMBER`**  
  Raised for invalid number formats.

- **`ERROR_CONSECUTIVE_OPERATORS`**  
  Raised when consecutive operators are found without operands.

- **`ERROR_INVALID_IDENTIFIER`**  
  Raised for invalid variable names.

### Parser Errors
- **`PARSE_ERROR_UNEXPECTED_TOKEN`**  
  Raised when the parser encounters an unexpected token.

- **`PARSE_ERROR_MISSING_SEMICOLON`**  
  Raised when a semicolon is missing at the end of a statement.

- **`PARSE_ERROR_MISSING_IDENTIFIER`**  
  Raised when an identifier is expected but missing.

- **`PARSE_ERROR_MISSING_EQUALS`**  
  Raised when an equals sign is missing in an assignment.

- **`PARSE_ERROR_INVALID_EXPRESSION`**  
  Raised when an expression cannot be parsed.

- **`PARSE_ERROR_MISSING_PARENTHESIS`**  
  Raised when a parenthesis is expected but missing.

- **`PARSE_ERROR_BAD_PARENTHESIS`**  
  Raised for mismatched parentheses.

- **`PARSE_ERROR_MISSING_BLOCK`**  
  Raised when a block of code is missing braces.

- **`PARSE_ERROR_MISSING_UNTILS`**  
  Raised when the `until` keyword is missing in a repeat-until loop.

### Semantic Errors
- **`SEM_ERROR_UNDECLARED_VARIABLE`**  
  Raised when a variable is used without being declared.

- **`SEM_ERROR_REDECLARED_VARIABLE`**  
  Raised when a variable is redeclared in the same scope.

- **`SEM_ERROR_TYPE_MISMATCH`**  
  Raised for type mismatches in expressions or assignments.

- **`SEM_ERROR_UNINITIALIZED_VARIABLE`**  
  Raised when an uninitialized variable is used.

- **`SEM_ERROR_INVALID_OPERATION`**  
  Raised for invalid operations, such as unsupported binary operations.