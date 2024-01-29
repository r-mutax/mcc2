#pragma once

#include "mcc2.h"
#include <stdbool.h>

void tokenize(char* src);
void expect_token(TokenKind kind);
bool consume_token(TokenKind kind);

Token* consume_ident();
Token* expect_ident();

int expect_num();

bool is_eof();
bool is_type();
char* get_token_string(Token* tok);

Token* get_token();
void set_token(Token* tok);
