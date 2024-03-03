#pragma once
#include "mcc2.h"

Ident* declare_ident(Token* ident, IdentKind kind, Type* ty);
Ident* make_ident(Token* ident, IdentKind kind, Type* ty);
void register_ident(Ident* ident);
Ident* register_string_literal(Token* tok);
void register_struct_or_union_type(Type* type);
Ident* find_ident(Token* tok);
Label* find_label(Token* tok);
Type* find_struct_or_union_type(Token* tok);
Label* register_label(Token* tok);
void scope_in();
void scope_out();
int get_stack_size();
Scope* get_current_scope();
Scope* get_global_scope();
