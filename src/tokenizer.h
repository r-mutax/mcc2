#pragma once

#include "mcc2.h"
#include <stdbool.h>

Token* tokenize(char* path);
bool is_equal_token(Token* lhs, Token* rhs);
char* get_token_string(Token* tok);
Token* next_newline(Token* tok);
Token* next_token(Token* tok);
Token* copy_token(Token* tok);
Token* copy_token_list(Token* tok);
Token* copy_token_eol(Token* tok);
Token* get_tokens_tail(Token* tok);
