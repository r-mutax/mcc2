#include "mcc2.h"
#include <stdlib.h>

Type* ty_int;

void ty_init(){
    ty_int = calloc(1, sizeof(Type));
    ty_int->size = 8;
    ty_int->kind = TY_INT;
}

