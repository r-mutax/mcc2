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
            case TK_IDENT:
                {
                    Token* ident = cur->next;
                    Macro* m = find_macro(ident);
                    if(m){
                        Token* value = copy_token(m->value);
                        value->next = cur->next->next;
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
    m->value = copy_token(value);
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

// for debug
static void print_macros(){
    Macro* m;
    for(m = macros; m; m = m->next){
        fprintf(stderr, "macro: %s\n", get_token_string(m->name));
        fprintf(stderr, "value: %s\n", get_token_string(m->value));
        fprintf(stderr, "\n");
    }
}
