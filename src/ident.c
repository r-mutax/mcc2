#include "mcc2.h"
#include "type.h"
#include "utility.h"
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

static Scope global_scope = {};
static Scope* cur_scope = &global_scope;
static Scope* func_scope = NULL;
static int stack_size = 0;

static int string_literal_num = 0;



Ident* declare_ident(Token* tok, IdentKind kind, Type* ty){
    Ident* ident = calloc(1, sizeof(Ident));

    if((ident->kind == ID_LVAR) && (cur_scope->level != 0)){
        if(ty->kind == TY_ARRAY){
            stack_size += ty->size * ty->array_len;    
        } else {
            stack_size += ty->size;
        }
    }

    ident->kind = kind;
    ident->name = strnewcpyn(tok->pos, tok->len);
    ident->tok = tok;
    ident->offset = stack_size;
    ident->type = ty;


    ident->next = cur_scope->ident;
    cur_scope->ident = ident;
    return ident;
}

Ident* register_string_literal(Token* tok){
    StringLiteral* sl = calloc(1, sizeof(StringLiteral));
    sl->name = calloc(1, sizeof(20));
    sprintf(sl->name, ".LSTR%d", string_literal_num++);
    sl->val = tok;

    sl->next = global_scope.string_literal;
    global_scope.string_literal = sl;

    Ident* ident = calloc(1, sizeof(Ident));
    ident->kind = ID_GVAR;
    ident->name = sl->name;
    ident->tok = sl->val;
    ident->is_string_literal = 1;
    ident->type = array_of(ty_char, sl->val->len + 1);

    ident->next = global_scope.ident;
    global_scope.ident = ident;
    return ident;
}

Label* register_label(Token* tok){
    Label* label = calloc(1, sizeof(Label));
    label->tok = tok;
    label->next = func_scope->label;
    func_scope->label = label;
    return label;
}

Ident* find_ident(Token* tok){
    for(Scope* sc = cur_scope; sc; sc = sc->parent){
        for(Ident* id = sc->ident; id; id = id->next){
            Token* lhs = tok;
            if((lhs->len == strlen(id->name))
                  && (!memcmp(lhs->pos, id->name, lhs->len))){
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

    if(cur_scope->level == 0){
        func_scope = new_scope;
    }

    cur_scope = new_scope;
}

void scope_out(){
    if(cur_scope->level == 0) return;
    if(cur_scope->level == 1) func_scope = NULL;

    Scope* buf = cur_scope;
    cur_scope = cur_scope->parent;

    if(cur_scope == &global_scope){
        stack_size = 0;
    }
}

Scope* get_current_scope(){
    return cur_scope;
}

Scope* get_global_scope(){
    return &global_scope;
}