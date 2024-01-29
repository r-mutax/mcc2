#include "mcc2.h"
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static Scope global_scope = {};
static Scope* cur_scope = &global_scope;
static int stack_size = 0;

Ident* declare_ident(Token* tok, IdentKind kind, Type* ty){
    Ident* ident = calloc(1, sizeof(Ident));

    ident->kind = kind;
    ident->tok = tok;
    ident->offset = stack_size + 8;
    ident->type = ty;

    if((ident->kind == ID_LVAR) && (cur_scope->level != 0)){
        if(ty->kind == TY_ARRAY){
            stack_size += ty->size * ty->array_len;    
        } else {
            stack_size += ty->size;
        }
    }

    ident->next = cur_scope->ident;
    cur_scope->ident = ident;
    return ident;
}

Ident* find_ident(Token* tok){
    for(Scope* sc = cur_scope; sc; sc = sc->parent){
        for(Ident* id = sc->ident; id; id = id->next){
            Token* lhs = tok;
            Token* rhs = id->tok;
            if((lhs->len == rhs->len)
                  && (!memcmp(lhs->pos, rhs->pos, lhs->len))){
                return id;
            }
        } 
    }

    return NULL;
}

int get_stack_size(){
    return stack_size;
}

void scope_in(){
    Scope* new_scope = calloc(1, sizeof(Scope));
    new_scope->level = cur_scope->level + 1;
    new_scope->parent = cur_scope;
    cur_scope = new_scope;
}

void scope_out(){
    if(cur_scope->level == 0) return;

    Scope* buf = cur_scope;
    cur_scope = cur_scope->parent;

    if(cur_scope == &global_scope){
        stack_size = 0;
    }
}

Scope* get_global_scope(){
    return &global_scope;
}