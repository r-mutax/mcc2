#pragma once

#include "mcc2.h"
#include <stdbool.h>

Token* tokenize(char* path);
bool is_equal_token(Token* lhs, Token* rhs);
char* get_token_string(Token* tok);
Token* next_newline(Token* tok);
Token* copy_token(Token* tok);
