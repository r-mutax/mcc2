#include "tokenizer.h"
#include "error.h"
#include "file.h"
#include "preprocess.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "keyword_map.h"

Token* token;
SrcFile* cur_file;

static Token* scan(char* src);
static Token* new_token(TokenKind kind, Token* cur, char* p);
static bool is_ident1(char c);
static bool is_ident2(char c);
static TokenKind    check_keyword(char* p, int len);
static TokenKind check_preprocess_keyword(char* p, int len);
Token* delete_newline_token(Token* tok);

Token* tokenize(char* path){
    SrcFile* file = read_file(path);
    cur_file = file;

    Token* tok = scan(file->body);
    tok = preprocess(tok);
    tok = delete_newline_token(tok);
    return tok;
}

static Token* scan(char* src){
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
                    cur = new_token(TK_PLUS_PLUS, cur, p);
                    p += 2;
                } else if(*(p + 1) == '=') {
                    cur = new_token(TK_PLUS_EQUAL, cur, p);
                    p += 2;
                } else {
                    cur = new_token(TK_PLUS, cur, p++);
                }
                break;
            case '-':
                if(*(p + 1) == '-'){
                    cur = new_token(TK_MINUS_MINUS, cur, p);
                    p += 2;
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
                } else if(*(p + 1) == '/') {
                    p += 2;
                    while(*p != '\n') ++p;
                } else if(*(p + 1) == '*') {
                    char* q = strstr(p + 2, "*/");
                    if(q == 0) {
                        // error_at(p, "Block comment is not close.");
                    }
                    p = q + 2;
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
            case '[':
                cur = new_token(TK_L_SQUARE_BRACKET, cur, p++);
                break;
            case ']':
                cur = new_token(TK_R_SQUARE_BRACKET, cur, p++);
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
                    // error_at(p, "error: unexpected token.\n");
                }
                break;
            case '?':
                cur = new_token(TK_QUESTION, cur, p++);
                break;
            case ':':
                cur = new_token(TK_COLON, cur, p++);
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
            case ',':
                cur = new_token(TK_COMMA, cur, p++);
                break;
            case '"':
                {
                    char* start = ++p;
                    while(*p != '"'){
                        p++;
                    }
                    cur = new_token(TK_STRING_LITERAL, cur, start);
                    cur->len = p - start;
                    p++;
                }
                break;
            case '\'':
                {
                    char a = *(++p);
                    cur = new_token(TK_NUM, cur, p);
                    cur->val = a;
                    while(*p != '\''){
                        p++;
                    }
                    p++;
                }
                break;
            case '.':
                if(*(p+1) == '.' && *(p+2) == '.' ){
                    cur = new_token(TK_DOT_DOT_DOT, cur, p);
                    p += 3;
                } else {
                    cur = new_token(TK_DOT, cur, p++);
                }
                break;
            case '\n':
                cur = new_token(TK_NEWLINE, cur, p++);
                break;
            case '#':
                {
                    if(*(p+1) == '#'){
                        cur = new_token(TK_HASH_HASH, cur, p);
                        p += 2;
                    } else if(is_ident1(*(p+1))){
                        char* hash = p;
                        char* s = p + 1;
                        p += 2;
                        while(is_ident2(*p)) {
                            p++;
                        }
                        TokenKind kind = check_preprocess_keyword(s, p - s);
                        if(kind == TK_IDENT){
                            cur = new_token(TK_HASH, cur, hash);
                            cur = new_token(TK_IDENT, cur, s);
                            cur->len = p - s;
                        } else {
                            cur = new_token(kind, cur, s);
                            cur->len = hash - s;
                        }
                    } else {
                        cur = new_token(TK_HASH, cur, p++);
                    }
                }
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
                    // error_at(p, "error: unexpected token.\n");
                }
                break;
        }
    }

    return head.next;
}


static Token* new_token(TokenKind kind, Token* cur, char* p){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->pos = p;
    tok->file = cur_file;
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

static TokenKind check_preprocess_keyword(char* p, int len){
    for(int i = 0; i < sizeof(preprocess_keyword_map) / sizeof(KEYWORD_MAP); i++){
        if(len == strlen(preprocess_keyword_map[i].keyword)
            && (!memcmp(p, preprocess_keyword_map[i].keyword, len))){
            return preprocess_keyword_map[i].kind;
        }
    }

    // keyword_mapになかった場合、トークンは識別子
    return TK_IDENT;
}

bool is_equal_token(Token* lhs, Token* rhs){
    if((lhs->len == rhs->len)
        && (!memcmp(lhs->pos, rhs->pos, lhs->len))){
                return true;
    }
    return false;
}

char* get_token_string(Token* tok){
    char* str = calloc(1, sizeof(char) * tok->len);
    memcpy(str, tok->pos, tok->len);
    return str;
}

Token* delete_newline_token(Token* tok){
    Token head;
    head.next = tok;
    Token* cur = &head;
    while(cur->next){
        if(cur->next->kind == TK_NEWLINE){
            cur->next = cur->next->next;
        } else {
            cur = cur->next;
        }
    }
    return head.next;
}

Token* next_newline(Token* tok){
    while(tok->kind != TK_NEWLINE){
        tok = tok->next;
    }
    return tok;
}
