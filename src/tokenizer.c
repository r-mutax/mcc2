#include "tokenizer.h"
#include "error.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "keyword_map.h"

Token* token;

static Token* new_token(TokenKind kind, Token* cur, char* p);
static bool is_ident1(char c);
static bool is_ident2(char c);
static TokenKind    check_keyword(char* p, int len);

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

                } else if(*(p + 1) == '=') {
                    cur = new_token(TK_PLUS_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_PLUS, cur, p++);
                }
                break;
            case '-':
                if(*(p + 1) == '-'){

                } else if(*(p + 1) == '=') {
                    cur = new_token(TK_MINUS_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_MINUS, cur, p++);
                }
                break;
            case '*':
                if(*(p + 1) == '='){
                    cur = new_token(TK_MUL_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_MUL, cur, p++);
                }
                break;
            case '/':
                if(*(p + 1) == '='){
                    cur = new_token(TK_DIV_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_DIV, cur, p++);
                }
                break;
            case '%':
                if(*(p + 1) == '='){
                    cur = new_token(TK_PERCENT_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_PERCENT, cur, p++);
                }
                break;
            case '(':
                cur = new_token(TK_L_PAREN, cur, p++);
                break;
            case ')':
                cur = new_token(TK_R_PAREN, cur, p++);
                break;
            case '{':
                cur = new_token(TK_L_BRACKET, cur, p++);
                break;
            case '}':
                cur = new_token(TK_R_BRACKET, cur, p++);
                break;
            case '&':
                if(*(p + 1) == '&'){
                    cur = new_token(TK_AND_AND, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_AND, cur, p++);
                }
                break;
            case '^':
                cur = new_token(TK_HAT, cur, p++);
                break;
            case '|':
                if(*(p + 1) == '|'){
                    cur = new_token(TK_PIPE_PIPE, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_PIPE, cur, p++);
                }
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
            case '?':
                cur = new_token(TK_QUESTION, cur, p++);
                break;
            case ':':
                cur = new_token(TK_CORON, cur, p++);
                break;
            case '<':
                if(*(p + 1) == '='){
                    cur = new_token(TK_L_ANGLE_BRACKET_EQUAL, cur, p);
                    p += 2;
                } else if(*(p + 1) == '<') {
                    if(*(p + 2) == '=') {
                        cur = new_token(TK_L_BITSHIFT_EQUAL, cur, p);
                        p += 3;
                    } else {
                        cur = new_token(TK_L_BITSHIFT, cur, p);
                        p += 2;
                    }
                } else {
                    cur = new_token(TK_L_ANGLE_BRACKET, cur, p++);
                }
                break;
            case '>':
                if(*(p + 1) == '='){
                    cur = new_token(TK_R_ANGLE_BRACKET_EQUAL, cur, p);
                    p += 2;
                } else if(*(p + 1) == '>') {
                    if(*(p + 2) == '=') {
                        cur = new_token(TK_R_BITSHIFT_EQUAL, cur, p);
                        p += 3;
                    } else {
                        cur = new_token(TK_R_BITSHIFT, cur, p);
                        p += 2;
                    }
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
                    
                    cur = new_token(check_keyword(s, p - s), cur, s);
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

Token* expect_ident(){
    Token* tok = consume_ident();
    if(tok == NULL){
        error_at(token->pos, "error: not a ident.\n", token->pos);
    }
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

static TokenKind check_keyword(char* p, int len){
    for(int i = 0; i < sizeof(keyword_map) / sizeof(KEYWORD_MAP); i++){
        if(len == strlen(keyword_map[i].keyword)
            && (!memcmp(p, keyword_map[i].keyword, len))){
            return keyword_map[i].kind;
        }
    }

    // keyword_mapになかった場合、トークンは識別子
    return TK_IDENT;
}

char* get_token_string(Token* tok){
    char* str = calloc(1, sizeof(char) * tok->len);
    memcpy(str, tok->pos, tok->len);
    return str;
}
