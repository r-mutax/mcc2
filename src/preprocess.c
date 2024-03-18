#include "mcc2.h"
#include "tokenizer.h"
#include "utility.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

IncludePath* include_paths = NULL;
IncludePath* std_include_paths = NULL;

Macro* macros = NULL;

static char* find_include_file(char* filename);
static void add_macro(Token* name, Token* value);
static Macro* find_macro(Token* name);
static void delete_macro(Macro* m);

// if-group
static Token* read_if(Token* token);
static bool eval_if_cond(Token* token);

// for debug
static void print_macros();

Token* preprocess(Token* token){
    Token head = {};
    head.next = token;
    Token* cur = &head;
    while(cur->next){
        switch(cur->next->kind){
            case TK_INCLUDE:
                {
                    char* filepath = get_token_string(cur->next->next);
                    char* path = find_include_file(filepath);
                    if(!path){
                        error("file not found: %s", filepath);
                    }
                    Token* inlcude = tokenize(path);
                    Token* newline = next_newline(cur->next);
                    cur->next = inlcude;

                    // get tail of tokenlist from include
                    Token* tail = inlcude;
                    while(tail->next){
                        if(tail->next->kind != TK_EOF){
                            tail = tail->next;
                        } else {
                            break;
                        }
                    }
                    tail->next = newline->next;
                    continue;
                }
                break;
            case TK_DEFINE:
                {
                    Token* defsymbol = cur->next->next;
                    Token* defvalue = NULL;
                    if(defsymbol->next->kind == TK_NEWLINE){
                        // 今は空マクロは非対応
                        error("macro definition is empty");
                    } else {
                        defvalue = defsymbol->next;
                    }
                    add_macro(defsymbol, defvalue);
                    Token* newline = next_newline(defvalue);
                    cur->next = newline->next;
                    continue;
                }
                break;
            
            case TK_UNDEF:
                {
                    Token* undefsymbol = cur->next->next;
                    Macro* m = find_macro(undefsymbol);
                    if(m){
                        delete_macro(m);
                    }
                    Token* newline = next_newline(undefsymbol);
                    cur->next = newline->next;
                    continue;
                }
                break;
            case TK_PP_IF:
            case TK_PP_IFDEF:
            case TK_PP_IFNDEF:
                cur->next = read_if(cur->next);
                break;
            case TK_IDENT:
                {
                    Token* ident = cur->next;
                    Macro* m = find_macro(ident);
                    if(m){
                        Token* value = copy_token_list(m->value);
                        Token* tail = get_tokens_tail(value);
                        tail->next = cur->next->next;
                        cur->next = value;
                    }                    
                }
                break;
            default:
                break;
        }
        cur = cur->next;
    }

    return head.next;
}

void add_include_path(char* path){
    IncludePath* p = malloc(sizeof(IncludePath));
    p->path = path;
    p->next = include_paths;
    include_paths = p;
}

static char* find_include_file(char* filename){
    IncludePath* p;
    for(p = include_paths; p; p = p->next){
        char* path = format_string("%s/%s", p->path, filename);
        if(fopen(path, "r")){
            return path;
        }
    }

    for(p = std_include_paths; p; p = p->next){
        char* path = format_string("%s/%s", p->path, filename);
        if(fopen(path, "r")){
            return path;
        }
    }

    return NULL;
}

static void add_macro(Token* name, Token* value){
    Macro* m = malloc(sizeof(Macro));
    m->name = copy_token(name);
    m->value = copy_token_eol(value);
    m->next = macros;
    macros = m;
}

static Macro* find_macro(Token* name){
    Macro* m;
    for(m = macros; m; m = m->next){
        if(is_equal_token(m->name, name)){
            return m;
        }
    }
    return NULL;
}

static void delete_macro(Macro* m){
    Macro* prev = NULL;
    Macro* cur = macros;
    while(cur){
        if(cur == m){
            if(prev){
                prev->next = cur->next;
            } else {
                macros = cur->next;
            }
            free(cur);
            return;
        }
        prev = cur;
        cur = cur->next;
    }

}

static Token* get_endif(Token* token){
    Token* cur = token;
    while(cur){
        switch(cur->kind){
            case TK_PP_ENDIF:
                return cur;
            case TK_PP_IF:
            case TK_PP_IFDEF:
            case TK_PP_IFNDEF:
                cur = get_endif(next_newline(cur));
                cur = next_newline(cur);
                break;
        }
        cur = cur->next;
    }
    return NULL;
}

static Token* read_if(Token* token){

    IF_GROUP if_head;
    IF_GROUP* if_group = &if_head;
    if_group->next = calloc(1, sizeof(IF_GROUP));
    if_group->cond = eval_if_cond(token);
    if_group->head = next_newline(token);

    Token* cur = next_newline(token);
    while(cur){
        Token* target = cur->next;
        bool endif = false;
        switch(cur->kind){
            case TK_PP_IF:
            case TK_PP_IFDEF:
            case TK_PP_IFNDEF:
                cur = get_endif(target);
                cur = next_newline(cur);
                break;
            case TK_PP_ELIF:
                if_group->tail = cur;
                if_group->next = calloc(1, sizeof(IF_GROUP));
                if_group->head = next_newline(target);
                if_group->cond = eval_if_cond(target);
                cur = next_newline(target);
                break;
            case TK_PP_ELSE:
                if_group->tail = cur;
                if_group->next = calloc(1, sizeof(IF_GROUP));
                if_group->head = next_newline(target);
                if_group->cond = true;
                cur = next_newline(target);
                break;
            case TK_PP_ENDIF:
                if_group->tail = cur;
                endif = true;
                cur = next_newline(target);
                break;
        }

        if(endif) break;
        cur = cur->next;
    }
    
    for(if_group = if_head.next; if_group->next; if_group = if_group->next){
        if(if_group->cond){
            if_group->tail->next = cur;
            return if_group->head;
        }
        if_group->tail->next = if_group->next->head;
    }
    return cur;
}

static bool eval_if_cond(Token* token){
    switch(token->kind){
        case TK_PP_IF:
            return true;
        case TK_PP_IFDEF:
            {
                Macro* m = find_macro(token->next);
                return m != NULL;
            }
            break;
        case TK_PP_IFNDEF:
            {
                Macro* m = find_macro(token->next);
                return m == NULL;
            }
            break;
        case TK_PP_ELSE:
            return true;
        default:
            error("invalid if condition");
            return false;
    }
}

// for debug
static void print_macros(){
    Macro* m;
    for(m = macros; m; m = m->next){
        fprintf(stderr, "macro: %s\n", get_token_string(m->name));
        fprintf(stderr, "value: %s\n", get_token_string(m->value));
        fprintf(stderr, "\n");
    }
}
