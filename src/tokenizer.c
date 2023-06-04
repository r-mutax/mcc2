#include "tokenizer.h"
#include <stdlib.h>
#include <ctype.h>

Token* token;

static Token* new_token(TokenKind kind, Token* cur, char* p);
static Token* next(char* p);

void tokenize(char* src){
    char* p = src;
    Token head = {};
    Token* cur = &head;

    /*
        次のトークンの1文字目で処理を分岐し、
        場合によっては2文字目、3文字目を先読みする。
        すぐに抜けたいときはcontinue_flgを立てる。
    */

    bool continue_flg = true;
    while(continue_flg){
        char c = *p;
        switch(c){
            case 0:
                continue_flg = false;
                cur = new_token(TK_EOF, cur, p++);
                break;
            case '+':
                if(*(p + 1) == '+'){

                } else {
                    cur = new_token(TK_ADD, cur, p++);
                }
                break;
            case '-':
                if(*(p + 1) == '-'){

                } else {
                    cur = new_token(TK_SUB, cur, p++);
                }
                break;
            default:
                if(isdigit(c)){
                    cur = new_token(TK_NUM, cur, p);
                    cur->val = strtol(p, &p, 10);
                } else if(isspace(c)){
                    // 何もしない
                }
                break;
        }
    }

    token = head.next;
}

void expect_token(TokenKind kind){
    if(token->kind != kind){
        exit(1);
    }

    token = token->next;
}

int expect_num(){
    if(token->kind != TK_NUM){
        exit(1);
    }

    int result = token->val;
    token = token->next;
    return result;
}

bool consume_token(TokenKind kind){
    if(token->kind != kind){
        return false;
    }
    token = token->next;
    return true;
}

static Token* new_token(TokenKind kind, Token* cur, char* p){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->pos = p;
    cur->next = tok;
    return tok;
}