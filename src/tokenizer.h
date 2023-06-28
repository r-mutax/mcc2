#pragma once

#include "mcc2.h"
#include <stdbool.h>

void tokenize(char* src);
void expect_token(TokenKind kind);
int expect_num();
bool consume_token(TokenKind kind);
Token* consume_ident();
Token* expect_ident();
bool is_eof();
char* get_token_string(Token* tok);
