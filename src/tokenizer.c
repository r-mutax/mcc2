#include "tokenizer.h"
#include "error.h"
#include <stdlib.h>
#include <ctype.h>

Token* token;

static Token* new_token(TokenKind kind, Token* cur, char* p);
static bool is_ident1(char c);
static bool is_ident2(char c);

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
            case '*':
                cur = new_token(TK_MUL, cur, p++);
                break;
            case '/':
                cur = new_token(TK_DIV, cur, p++);
                break;
            case '(':
                cur = new_token(TK_L_PAREN, cur, p++);
                break;
            case ')':
                cur = new_token(TK_R_PAREN, cur, p++);
                break;
            case '=':
                if(*(p + 1) == '='){
                    cur = new_token(TK_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_ASSIGN, cur, p++);
                }
                break;
            case '!':
                if(*(p + 1) == '='){
                    cur = new_token(TK_NOT_EQUAL, cur, p);
                    p += 2;
                } else {
                    // まだ使えないから
                    error_at(p, "error: unexpected token.\n");
                }
                break;
            case '<':
                if(*(p + 1) == '='){
                    cur = new_token(TK_L_ANGLE_BRACKET_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_L_ANGLE_BRACKET, cur, p++);
                }
                break;
            case '>':
                if(*(p + 1) == '='){
                    cur = new_token(TK_R_ANGLE_BRACKET_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_R_ANGLE_BRACKET, cur, p++);
                }
                break;
            case ';':
                cur = new_token(TK_SEMICORON, cur, p++);
                break;
            default:
                if(isdigit(c)){
                    cur = new_token(TK_NUM, cur, p);
                    cur->val = strtol(p, &p, 10);
                } else if(isspace(c)){
                    p++;
                } else if(is_ident1(c)){
                    char* s = p;
                    p++;
                    while(is_ident2(*p)) { 
                        p++;
                    }
                    
                    cur = new_token(TK_IDENT, cur, s);
                    cur->len = p - s;
                } else {
                    // 想定外のトークンが来た
                    error_at(p, "error: unexpected token.\n");
                }
                break;
        }
    }

    token = head.next;
}

void expect_token(TokenKind kind){
    if(token->kind != kind){
        error_at(token->pos, "error: unexpected token.\n");
    }

    token = token->next;
}

int expect_num(){
    if(token->kind != TK_NUM){
        error_at(token->pos, "error: not a number.\n", token->pos);
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

Token* consume_ident(){
    if(token->kind != TK_IDENT) return NULL;

    Token* tok = token;
    token = token->next;
    return tok;
}

bool is_eof(){
    return token->kind == TK_EOF;
}

static Token* new_token(TokenKind kind, Token* cur, char* p){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->pos = p;
    cur->next = tok;
    return tok;
}

static bool is_ident1(char c){
    return isalpha(c) || c == '_';
}

static bool is_ident2(char c){
    return is_ident1(c) || isdigit(c);
}
