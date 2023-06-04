#include "mcc2.h"
#include "tokenizer.h"
#include <stdlib.h>
#include <stdbool.h>

Token* tokens;

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
                break;
            case '+':
                if(*(p + 1) == '+'){

                } else {
                    cur = new_token(TK_ADD, cur, p);
                }
                break;
            case '-':
                if(*(p + 1) == '-'){

                } else {
                    cur = new_token(TK_ADD, cur, p);
                }
                break;
        }
        p++;
    }

    tokens = head.next;
}

static Token* new_token(TokenKind kind, Token* cur, char* p){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->pos = p;
    cur->next = tok;
    return tok;
}