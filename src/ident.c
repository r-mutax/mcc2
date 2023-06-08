#include "mcc2.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static Ident   head = {ID_EOI, NULL, 0, 0, 0, NULL};
static Ident*  cur_i = &head;
static int scope_level = 0;
static int stack_size = 0;

Ident* declare_ident(Token* tok, int size, IdentKind kind){
    Ident* ident = calloc(1, sizeof(Ident));

    ident->kind = kind;
    ident->tok = tok;
    ident->size = size;
    ident->offset = stack_size + 8;
    ident->level = scope_level;

    stack_size += 8;

    ident->next = cur_i;
    cur_i = ident;
    return ident;
}

Ident* find_ident(Token* tok){
    for(Ident* cur = cur_i; cur->kind != ID_EOI; cur = cur->next){
        Token* lhs = tok;
        Token* rhs = cur->tok;

        if((lhs->len == rhs->len)
            && (!memcmp(lhs->pos, rhs->pos, lhs->len))){
            return cur;
        }
    }
    return NULL;
}

int get_stack_size(){
    return stack_size;
}

void scope_in(){
    scope_level++;
}

void scope_out(){
    if(scope_level == 0) return;

    scope_level--;

    while(true){
        if(cur_i->level == scope_level){
            break;
        }
        cur_i = cur_i->next;
    }

    if(scope_level == 0){
        stack_size = 0;
    }
}