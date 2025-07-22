#include "mcc2.h"
#include "keyword_map.h"

Token* token;
SrcFile* src_files;
SrcFile* cur_file;
extern bool is_preprocess;
static long row = 0;
static char* row_start = NULL;

Token* scan(char* src);
static Token* new_token(TokenKind kind, Token* cur, char* p, int len);
static bool is_ident1(char c);
static bool is_ident2(char c);
static TokenKind    check_keyword(char* p, int len);
static TokenKind check_preprocess_keyword(char* p, int len);
static char read_escaped_char(char** p);
Token* delete_newline_token(Token* tok);


Token* tokenize(char* path){
    SrcFile* file = read_file(path);

    if(!src_files){
        src_files = file;
    } else {
        SrcFile* last = src_files;
        while(last->next) {
            last = last->next;
        }
        last->next = file;
    }

    cur_file = file;
    if(!cinfo.compile_file){
        cinfo.compile_file = file;
    }

    Token* tok = scan(file->body);
    tok = preprocess(tok);
    if(!is_preprocess)
        tok = delete_newline_token(tok);
    return tok;
}

Token* tokenize_string(char* src){
    cur_file = NULL;
    Token* tok;
    tok = scan(src);
    tok = preprocess(tok);
    if(!is_preprocess)
        tok = delete_newline_token(tok);
    return tok;
}

Token* scan(char* src){
    char* p = src;
    Token head = {};
    Token* cur = &head;

    row = 1;
    row_start = p;

    //    次のトークンの1文字目で処理を分岐し、
    //    場合によっては2文字目、3文字目を先読みする。
    //    すぐに抜けたいときはcontinue_flgを立てる。

    bool continue_flg = true;
    while(continue_flg){
        char c = *p;
        switch(c){
            case 0:
                continue_flg = false;
                cur = new_token(TK_EOF, cur, p++, 1);
                break;
            case '+':
                if(*(p + 1) == '+'){
                    cur = new_token(TK_PLUS_PLUS, cur, p, 2);
                    p += 2;
                } else if(*(p + 1) == '=') {
                    cur = new_token(TK_PLUS_EQUAL, cur, p, 2);
                    p += 2;
                } else {
                    cur = new_token(TK_PLUS, cur, p++, 1);
                }
                break;
            case '-':
                if(*(p + 1) == '-'){
                    cur = new_token(TK_MINUS_MINUS, cur, p, 2);
                    p += 2;
                } else if(*(p + 1) == '=') {
                    cur = new_token(TK_MINUS_EQUAL, cur, p, 2);
                    p += 2;
                } else if(*(p + 1) == '>') {
                    cur = new_token(TK_ARROW, cur, p, 2);
                    p += 2;
                } else {
                    cur = new_token(TK_MINUS, cur, p++, 1);
                }
                break;
            case '*':
                if(*(p + 1) == '='){
                    cur = new_token(TK_MUL_EQUAL, cur, p, 2);
                    p += 2;
                } else {
                    cur = new_token(TK_MUL, cur, p++, 1);
                }
                break;
            case '/':
                if(*(p + 1) == '='){
                    cur = new_token(TK_DIV_EQUAL, cur, p, 2);
                    p += 2;
                } else if(*(p + 1) == '/') {
                    p += 2;
                    while(*p != '\n') ++p;
                } else if(*(p + 1) == '*') {
                    char* q = strstr(p + 2, "*/");
                    if(q == 0) {
                        error_at_src(p, cur_file, "Block comment is not close.");
                    }
                    p = q + 2;
                } else {
                    cur = new_token(TK_DIV, cur, p++, 1);
                }
                break;
            case '%':
                if(*(p + 1) == '='){
                    cur = new_token(TK_PERCENT_EQUAL, cur, p, 2);
                    p += 2;
                } else {
                    cur = new_token(TK_PERCENT, cur, p++, 1);
                }
                break;
            case '(':
                cur = new_token(TK_L_PAREN, cur, p++, 1);
                break;
            case ')':
                cur = new_token(TK_R_PAREN, cur, p++, 1);
                break;
            case '{':
                cur = new_token(TK_L_BRACKET, cur, p++, 1);
                break;
            case '}':
                cur = new_token(TK_R_BRACKET, cur, p++, 1);
                break;
            case '[':
                cur = new_token(TK_L_SQUARE_BRACKET, cur, p++, 1);
                break;
            case ']':
                cur = new_token(TK_R_SQUARE_BRACKET, cur, p++ , 1);
                break;
            case '&':
                if(*(p + 1) == '&'){
                    cur = new_token(TK_AND_AND, cur, p , 2);
                    p += 2;
                } else {
                    cur = new_token(TK_AND, cur, p++ , 1);
                }
                break;
            case '^':
                cur = new_token(TK_HAT, cur, p++ , 1);
                break;
            case '|':
                if(*(p + 1) == '|'){
                    cur = new_token(TK_PIPE_PIPE, cur, p , 2);
                    p += 2;
                } else {
                    cur = new_token(TK_PIPE, cur, p++ , 1);
                }
                break;
            case '=':
                if(*(p + 1) == '='){
                    cur = new_token(TK_EQUAL, cur, p , 2);
                    p += 2;
                } else {
                    cur = new_token(TK_ASSIGN, cur, p++ , 1);
                }
                break;
            case '!':
                if(*(p + 1) == '='){
                    cur = new_token(TK_NOT_EQUAL, cur, p , 2);
                    p += 2;
                } else {
                    cur = new_token(TK_NOT, cur, p++ , 1);
                }
                break;
            case '?':
                cur = new_token(TK_QUESTION, cur, p++ , 1);
                break;
            case ':':
                cur = new_token(TK_COLON, cur, p++ , 1);
                break;
            case '<':
                if(*(p + 1) == '='){
                    cur = new_token(TK_L_ANGLE_BRACKET_EQUAL, cur, p , 2);
                    p += 2;
                } else if(*(p + 1) == '<') {
                    if(*(p + 2) == '=') {
                        cur = new_token(TK_L_BITSHIFT_EQUAL, cur, p, 3);
                        p += 3;
                    } else {
                        cur = new_token(TK_L_BITSHIFT, cur, p , 2);
                        p += 2;
                    }
                } else {
                    cur = new_token(TK_L_ANGLE_BRACKET, cur, p++ , 1);
                }
                break;
            case '>':
                if(*(p + 1) == '='){
                    cur = new_token(TK_R_ANGLE_BRACKET_EQUAL, cur, p , 2);
                    p += 2;
                } else if(*(p + 1) == '>') {
                    if(*(p + 2) == '=') {
                        cur = new_token(TK_R_BITSHIFT_EQUAL, cur, p, 3);
                        p += 3;
                    } else {
                        cur = new_token(TK_R_BITSHIFT, cur, p , 2);
                        p += 2;
                    }
                } else {
                    cur = new_token(TK_R_ANGLE_BRACKET, cur, p++ , 1);
                }
                break;
            case ';':
                cur = new_token(TK_SEMICORON, cur, p++ , 1);
                break;
            case ',':
                cur = new_token(TK_COMMA, cur, p++ , 1);
                break;
            case '"':
                {
                    char* start = ++p;
                    cur = new_token(TK_STRING_LITERAL, cur, start, 0);
                    while(*p != '"'){
                        if(*p == '\\'){
                            // エスケープシーケンスの分進める
                            read_escaped_char(&p);
                        } else if(*p == '\n'){
                            error_at_src(p, cur_file, "error: unclosed string literal.\n");
                        } else {
                            p++;
                        }
                    }

                    cur->len = p - start;
                    cur->str = strnewcpyn(start, cur->len);
                    p++;
                }
                break;
            case '\'':
                {
                    char* pos = ++p;
                    char a = *p;
                    if(a == '\\'){
                        // escape sequance
                        a = read_escaped_char(&p);
                    }
                    cur = new_token(TK_NUM, cur, pos, 0);
                    cur->val = a;

                    // 文字リテラルの終わり
                    while(*p != '\''){
                        p++;
                        if(*p == 0){
                            error_at_src(p, cur_file, "error: unclosed char literal.\n");
                        }
                    }
                    p++;
                    cur->len = p - cur->pos;
                }
                break;
            case '.':
                if(*(p+1) == '.' && *(p+2) == '.' ){
                    cur = new_token(TK_DOT_DOT_DOT, cur, p, 3);
                    p += 3;
                } else {
                    cur = new_token(TK_DOT, cur, p++ , 1);
                }
                break;
            case '\n':
                cur = new_token(TK_NEWLINE, cur, p++ , 1);
                row++;
                row_start = p;
                break;
            case '#':
                {
                    if(*(p+1) == '#'){
                        cur = new_token(TK_HASH_HASH, cur, p, 2);
                        p += 2;
                    } else {
                        // space除去
                        while(isspace(*p)){
                            p++;
                        }

                        if(is_ident1(*(p+1))){
                            char* hash = p;
                            char* s = p + 1;
                            p += 2;
                            while(is_ident2(*p)) {
                                p++;
                            }
                            TokenKind kind = check_preprocess_keyword(s, p - s);
                            if(kind == TK_IDENT){
                                cur = new_token(TK_HASH, cur, hash, 1);
                                cur = new_token(TK_IDENT, cur, s, 0);
                                cur->len = p - s;
                            } else {
                                cur = new_token(TK_HASH, cur, hash, 1);
                                cur = new_token(kind, cur, s, 0);
                                cur->len = hash - s;
                            }
                        } else {
                            cur = new_token(TK_HASH, cur, p++ , 1);
                        }
                    }
                }
                break;
            default:
                if(isdigit(c)){
                    if(c == '0' && (*(p + 1) == 'x' || *(p + 1) == 'X')){
                        // 16進数
                        p += 2;
                        cur = new_token(TK_NUM, cur, p - 2, 0);
                        cur->val = strtoul(p, &p, 16);
                        cur->len = p - cur->pos;
                    } else if(c == '0' && (*(p + 1) == 'b' || *(p + 1) == 'B')){
                        // 2進数
                        p += 2;
                        cur = new_token(TK_NUM, cur, p - 2, 0);
                        cur->val = strtoul(p, &p, 2);
                        cur->len = p - cur->pos;
                    } else if(c == '0'){
                        // 8進数
                        p++;
                        cur = new_token(TK_NUM, cur, p - 1, 0);
                        cur->val = strtoul(p, &p, 8);
                        cur->len = p - cur->pos;
                    } else {
                        // 10進数
                        cur = new_token(TK_NUM, cur, p, 0);
                        cur->val = strtoul(p, &p, 10);
                        cur->len = p - cur->pos;
                    }
                    //    1 : 'u' 'l'opt
                    //    2 : 'u' 'll'opt
                    //    3 : 'l' 'u'opt
                    //    4 : 'll' 'u'opt
                    //    なら読み飛ばす

                    if(toupper(*p) == 'U'){
                        p++;
                        if(toupper(*p) == 'L'){
                            p++;
                            if(toupper(*p) == 'L'){
                                p++;
                            }
                        }
                    } else if(toupper(*p) == 'L'){
                        p++;
                        if(toupper(*p) == 'U'){
                            p++;
                        } else if(toupper(*p) == 'L'){
                            p++;
                            if(toupper(*p) == 'U'){
                                p++;
                            }
                        }
                    }
                } else if(isspace(c)){
                    cur = new_token(TK_SPACE, cur, p++ , 1);
                    // p++;
                } else if(is_ident1(c)){
                    char* s = p;
                    p++;
                    while(is_ident2(*p)) {
                        p++;
                    }
                    cur = new_token(check_keyword(s, p - s), cur, s, 0);
                    cur->len = p - s;
                } else {
                    // 想定外のトークンが来た
                    error_at_src(p, cur_file, "error: unexpected token.\n");
                }
                break;
        }
    }

    return head.next;
}


static Token* new_token(TokenKind kind, Token* cur, char* p, int len){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->pos = p;
    tok->file = cur_file;
    tok->len = len;
    tok->row = row;
    tok->col = p - row_start + 1;
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

static char read_escaped_char(char** p){
    char c = *++*p;
    switch(c){
        case 'a':
            ++*p;
            return '\a';
        case 'b':
            ++*p;
            return '\b';
        case 'f':
            ++*p;
            return '\f';
        case 'n':
            ++*p;
            return '\n';
        case 'r':
            ++*p;
            return '\r';
        case 't':
            ++*p;
            return '\t';
        case 'v':
            ++*p;
            return '\v';
        case 'e':
            ++*p;
            return '\e';
        case '\\':
            ++*p;
            return '\\';
        case '\'':
            ++*p;
            return '\'';
        case '\"':
            ++*p;
            return '\"';
        case '?':
            ++*p;
            return '\?';
        case '0':
            ++*p;
            return '\0';
        default:
            error_at_src(*p, cur_file, "invalid escape character.\n");
            return 0;
    }
}

bool is_equal_token(Token* lhs, Token* rhs){
    if((lhs->len == rhs->len)
        && (!memcmp(lhs->pos, rhs->pos, lhs->len))){
                return true;
    }
    return false;
}

char* get_token_string(Token* tok){
    char* str = calloc(1, sizeof(char) * tok->len + 1);
    memcpy(str, tok->pos, tok->len);
    return str;
}

char* get_token_string_literal(Token* tok){
    char* str = calloc(1, sizeof(char) * tok->len + 1);
    memcpy(str, tok->str, tok->len);
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

Token* next_token(Token* tok){
    for(Token* cur = tok->next; cur; cur = cur->next){
        if(cur->kind != TK_SPACE){
            return cur;
        }
    }
    return  NULL;
}

Token* skip_to_next(Token* tok, TokenKind kind){
    while(tok->kind != kind){
        tok = tok->next;
        if(!tok){
            return NULL;
        }
    }
    return tok;
}

Token* copy_token(Token* tok){
    Token* new = calloc(1, sizeof(Token));
    memcpy(new, tok, sizeof(Token));
    new->next = NULL;
    return new;
}

Token* copy_token_list(Token* tok){
    Token head;
    Token* cur = &head;
    while(tok){
        cur->next = copy_token(tok);
        cur = cur->next;
        tok = tok->next;
    }
    return head.next;
}

// 改行までのトークンのつながりをコピーする
Token* copy_token_eol(Token* tok){
    Token head;
    Token* cur = &head;
    while(tok->kind != TK_NEWLINE){
        cur->next = copy_token(tok);
        cur = cur->next;
        tok = tok->next;
    }
    return head.next;
}

Token* get_tokens_tail(Token* tok){
    while(tok->next){
        tok = tok->next;
    }
    return tok;
}

void output_token(Token* tok){
    while(tok){
        if(tok->kind == TK_STRING_LITERAL){
            print("\"%.*s\"", tok->len, tok->pos);
        } else {
            print("%.*s", tok->len, tok->pos);
        }
        tok = tok->next;
    }
}

Token* get_token_before_eof(Token* tok){
    while(tok->next->kind != TK_EOF){
        tok = tok->next;
    }
    return tok;
}

Token* create_token(TokenKind kind, char* str, int len){
    Token* tok = calloc(1, sizeof(Token));
    tok->kind = kind;
    tok->pos = str;
    tok->len = len;
    return tok;
}

// 指定名のヘッダを何回読み込んだか数える
int count_include_file(char* filename){
    int count = 0;
    SrcFile* file = src_files;
    while(file){
        if(strcmp(file->name, filename) == 0){
            count++;
        }
        file = file->next;
    }
    return count;
}