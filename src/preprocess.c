#include "mcc2.h"
#include "tokenizer.h"
#include "utility.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

IncludePath* include_paths = NULL;
IncludePath* std_include_paths = NULL;

static char* find_include_file(char* filename);

Token* preprocess(Token* token){
    Token head = {};
    head.next = token;
    Token* cur = &head;
    while(cur->next){
        switch(cur->next->kind){
            case TK_INCLUDE:
                {
                    char* filepath = get_token_string(cur->next->next);
                    fprintf(stderr, "include: %s\n", filepath);
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
