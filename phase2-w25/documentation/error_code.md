# Parser Error Handling

Parser handles various syntax errors through different error codes.
For error handling, when the parser encounters an error, it will immediately stop parsing the input, and generate an error message for the user and then exit.
Each error message is printed along with the line and column number of the token where the error was detected to help with debugging and error recovery.

## PARSE_ERROR_UNEXPECTED_TOKEN
- When parser encounters a token that does not match the expected typ.

## PARSE_ERROR_MISSING_SEMICOLON
- When semicolon is expected at the end of a statement, but missing

## PARSE_ERROR_MISSING_IDENTIFIER
- When identifier is expected, but missing

## PARSE_ERROR_MISSING_EQUALS
- When equals is missing in an assigment statement

## PARSE_ERROR_INVALID_EXPRESSION
- When expression can not be parsed

## PARSE_ERROR_MISSING_PARENTHESIS
- When parenthesis is expected, but missing

## PARSE_ERROR_BAD_PARENTHESIS
- When there is a mismatch of parenthesis 

## PARSE_ERROR_MISSING_BLOCK
- When code which is expected to be enclosed in braces, but braces are missing

## PARSE_ERROR_MISSING_UNTILS
- When `repeat-until` loop is missing `until` keyword is missing after statement block

