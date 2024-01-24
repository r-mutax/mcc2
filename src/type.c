#include "mcc2.h"
#include <stdlib.h>

Type* ty_int;
Type* new_type(TypeKind kind, int size);

void ty_init(){
    ty_int = new_type(TY_INT, 8);
}

Type* pointer_to(Type* base){
    Type* ty = new_type(TY_POINTER, 8);
    ty->ptr_to = base;
    ty->is_unsigned = 1;
    return ty;
}

Type* new_type(TypeKind kind, int size){
    Type* ty = calloc(1, sizeof(Type));
    ty->kind = kind;
    ty->size = size;
    return ty;
}