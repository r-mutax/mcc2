#pragma once
#include "mcc2.h"
#include "ident.h"

extern Type* ty_int;
extern Type* ty_char;
extern Type* ty_short;
extern Type* ty_long;
extern Type* ty_uchar;
extern Type* ty_ushort;
extern Type* ty_uint;
extern Type* ty_ulong;

void ty_init();
Type* copy_type(Type* type);
Type* pointer_to(Type* base);
Type* array_of(Type* base, int len);
void add_type(Node* node);
bool equal_type(Type* ty1, Type* ty2);
Type* new_type(TypeKind kind, int size);
Ident* get_member(Type* type, Token* tok);
