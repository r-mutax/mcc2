#pragma once

Ident* declare_ident(Token* ident, int size, IdentKind kind);
Ident* find_ident(Token* tok);
void scope_in();
void scope_out();
int get_stack_size();
