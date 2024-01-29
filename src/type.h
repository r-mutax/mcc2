#pragma once
#include "mcc2.h"

extern Type* ty_int;
extern Type* ty_char;

void ty_init();
Type* pointer_to(Type* base);
Type* array_of(Type* base, int len);
void add_type(Node* node);

