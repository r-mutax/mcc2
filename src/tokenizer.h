#pragma once

#include "mcc2.h"
#include <stdbool.h>

Token* tokenize(char* path);
void set_token(Token* tok);

void tokenize_file(char* path);
void expect_token(TokenKind kind);
bool consume_token(TokenKind kind);

Token* consume_ident();
Token* expect_ident();

Token* consume_string_literal();

int expect_num();

bool is_eof();
bool is_type();
bool is_label();
bool is_equal_token(Token* lhs, Token* rhs);
char* get_token_string(Token* tok);

Token* get_token();
void set_token(Token* tok);
