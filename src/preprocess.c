#include "mcc2.h"
#include "tokenizer.h"
#include "utility.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

IncludePath* include_paths = NULL;
IncludePath* std_include_paths = NULL;

Token tok_zero = {
    .kind = TK_NUM,
    .val = 0,
};

Token tok_one = {
    .kind = TK_NUM,
    .val = 1,
};

Macro* macros = NULL;

static char* find_include_file(char* filename);
static void add_macro(Token* name, Token* value);
static Macro* find_macro(Token* name);
static void delete_macro(Macro* m);
static Token* delete_space(Token* token);

// if-group
static Token* read_if(Token* token);
static bool eval_if_cond(Token* token);
static bool eval_expr(Token* token);
static Token* expand_defined(Token* tok);

// constant_expr for preprocessor
Token* expr_token = NULL;
static int pp_constant_expr(Token* token);
static int pp_expr();
static int pp_cond_expr();
static int pp_logicOr();
static int pp_logicAnd();
static int pp_bitOr();
static int pp_bitXor();
static int pp_bitAnd();
static int pp_equality();
static int pp_relational();
static int pp_bitShift();
static int pp_add();
static int pp_mul();
static int pp_unary();
static int pp_primary();

static bool pp_consume(TokenKind kind);
static int get_token_int(Token* token);

/*
    expr = cond_expr
    cond_expr = logicOR ( '?' expr : cond_expr );
    logicOr = logicAnd ( '||' logicOr )*
    logicAnd = bitOr ( '&&' bitOr )*
    bitOr = bitXor ('|' bitXor)*
    bitXor = bitAnd ('^' bitAnd )*
    bitAnd = equality ('&' equality )*
    equality = relational ('==' relational | '!=' relational)*
    relational = bitShift ('<' bitShift | '<=' bitShift | '>' bitShift | '>=' bitShift)*
    bitShift = add ('<<' add | '>>' add)?
    add = mul ('+' mul | '-' mul)*
    mul = unary ('*' unary | '/' unary)
    unary = ('+' | '-' | '!' )? primary
    primary = '(' expr ')' | num
*/


// for debug
static void print_macros();

Token* preprocess(Token* token){
    Token head = {};
    head.next = token;
    Token* cur = &head;
    while(next_token(cur)){
        Token* t1 = next_token(cur);
        if(t1->kind == TK_HASH){
            Token* target = next_token(t1);
            switch(target->kind){
                case TK_INCLUDE:
                    {
                        char* filepath = get_token_string(next_token(target));
                        char* path = find_include_file(filepath);
                        if(!path){
                            error("file not found: %s", filepath);
                        }
                        Token* inlcude = tokenize(path);
                        Token* newline = next_newline(target);
                        cur->next = inlcude;

                        // get tail of tokenlist from include
                        Token* tail = inlcude;
                        while(next_token(tail)){
                            if(next_token(tail)->kind != TK_EOF){
                                tail = next_token(tail);
                            } else {
                                break;
                            }
                        }
                        tail->next = next_token(newline);
                        continue;
                    }
                    break;
                case TK_DEFINE:
                    {
                        Token* defsymbol = next_token(target);
                        Token* defvalue = NULL;
                        if(next_token(defsymbol)->kind == TK_NEWLINE){
                            // 今は空マクロは非対応
                            error("macro definition is empty");
                        } else {
                            defvalue = next_token(defsymbol);
                        }
                        add_macro(defsymbol, defvalue);
                        Token* newline = next_newline(defvalue);
                        cur->next = next_token(newline);
                        continue;
                    }
                    break;
                case TK_UNDEF:
                    {
                        Token* undefsymbol = next_token(target);
                        Macro* m = find_macro(undefsymbol);
                        if(m){
                            delete_macro(m);
                        }
                        Token* newline = next_newline(undefsymbol);
                        cur->next = next_token(newline);
                        continue;
                    }
                    break;
                case TK_PP_IF:
                case TK_PP_IFDEF:
                case TK_PP_IFNDEF:
                    cur->next = read_if(target);
                    break;
                default:
                    break;
            }
        } else if(next_token(cur)->kind == TK_IDENT){
            Token* ident = next_token(cur);
            Macro* m = find_macro(ident);
            if(m){
                Token* value = copy_token_list(m->value);
                Token* tail = get_tokens_tail(value);
                tail->next = t1->next;
                cur->next = value;
            }
        }

        cur = cur->next;
    }

    head.next = delete_space(head.next);

    return head.next;
}

void add_include_path(char* path){
    IncludePath* p = malloc(sizeof(IncludePath));
    p->path = path;
    p->next = include_paths;
    include_paths = p;
}

void add_predefine_macro(char* name){
    Token* tok = calloc(1, sizeof(Token));
    tok->pos = strnewcpyn(name, strlen(name));
    tok->len = strlen(name);
    tok->kind = TK_IDENT;

    Macro* m = malloc(sizeof(Macro));
    m->name = tok;
    m->value = NULL;
    m->next = macros;
    macros = m;
}

static Token* delete_space(Token* token){
    Token head = {};
    head.next = token;
    Token* cur = &head;

    while(cur->next){
        Token* target = cur->next;
        if(target->kind == TK_SPACE){
            cur->next = target->next;
        } else {
            cur = cur->next;
        }
    }

    return head.next;

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
    if_head.cond = false;

    IF_GROUP* if_group = &if_head;
    if_group = if_group->next = calloc(1, sizeof(IF_GROUP));
    if_group->cond = eval_if_cond(token);
    if_group->head = next_newline(token);

    Token* cur = next_newline(token);
    while(cur){
        Token* t1 = next_token(cur);
        if(t1->kind == TK_HASH){
            Token* target = next_token(t1);
            bool endif = false;
            switch(target->kind){
                case TK_PP_IF:
                case TK_PP_IFDEF:
                case TK_PP_IFNDEF:
                    cur = get_endif(target->next);
                    cur = next_newline(cur);
                    break;
                case TK_PP_ELIF:
                    if_group->tail = cur;
                    if_group = if_group->next = calloc(1, sizeof(IF_GROUP));
                    if_group->head = next_newline(target);
                    if_group->cond = eval_if_cond(target);
                    cur = next_newline(target);
                    break;
                case TK_PP_ELSE:
                    if_group->tail = cur;
                    if_group = if_group->next = calloc(1, sizeof(IF_GROUP));
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
        } else {
            cur = cur->next;
        }
    }
    for(if_group = &if_head; if_group; if_group = if_group->next){
        if(if_group->cond){
            if_group->tail->next = cur;
            return if_group->head;
        }
    }
    return cur;
}

static bool eval_if_cond(Token* token){
    switch(token->kind){
        case TK_PP_IF:
            return eval_expr(copy_token_eol(next_token(token)));
        case TK_PP_ELIF:
            return eval_expr(copy_token_eol(next_token(token)));
        case TK_PP_IFDEF:
            {
                Macro* m = find_macro(next_token(token));
                return m != NULL;
            }
            break;
        case TK_PP_IFNDEF:
            {
                Macro* m = find_macro(next_token(token));
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

static Token* expand_defined(Token* tok){
    Token head = {};
    Token *cur = &head;
    cur->next = tok;

    while(cur && cur->next){
        Token* target = next_token(cur);
        if(target->kind == TK_DEFINED){
            Token* ident = NULL;
            target = next_token(target);

            if(target->kind == TK_L_PAREN){
                ident = target = next_token(target);
                target = next_token(target);
                if(target->kind != TK_R_PAREN){
                    error("invalid defined");
                }
            } else {
                ident = target;
            }
            target = next_token(target);

            cur->next = find_macro(ident) ? &tok_one : &tok_zero;
            cur->next->next = target;
        } else {
            cur = next_token(cur);
        }
    }
    return head.next;
}

static bool eval_expr(Token* token){
    token = expand_defined(token);
    return pp_constant_expr(token);
}

static int pp_constant_expr(Token* token){
    expr_token = token;
    return pp_expr();
}

static int pp_expr(){
    return pp_cond_expr();
}

static int pp_cond_expr(){
    int val = pp_logicOr();
    if(pp_consume(TK_QUESTION)){
        int then = pp_expr();
        pp_consume(TK_COLON);
        int els = pp_cond_expr();
        return val ? then : els;
    }
    return val;
}

static int pp_logicOr(){
    int val = pp_logicAnd();
    while(pp_consume(TK_PIPE_PIPE)){
        val = val || pp_logicAnd();
    }
    return val;
}

static int pp_logicAnd(){
    int val = pp_bitOr();
    while(pp_consume(TK_AND_AND)){
        val = val && pp_bitOr();
    }
    return val;
}

static int pp_bitOr(){
    int val = pp_bitXor();
    while(pp_consume(TK_PIPE)){
        val = val | pp_bitXor();
    }
    return val;
}

static int pp_bitXor(){
    int val = pp_bitAnd();
    while(pp_consume(TK_HAT)){
        val = val ^ pp_bitAnd();
    }
    return val;
}

static int pp_bitAnd(){
    int val = pp_equality();
    while(pp_consume(TK_AND)){
        val = val & pp_equality();
    }
    return val;
}

static int pp_equality(){
    int val = pp_relational();
    while(1){
        if(pp_consume(TK_EQUAL)){
            val = val == pp_relational();
        } else if(pp_consume(TK_NOT_EQUAL)){
            val = val != pp_relational();
        } else {
            break;
        }
    }
    return val;
}

static int pp_relational(){
    int val = pp_bitShift();
    while(1){
        if(pp_consume(TK_L_ANGLE_BRACKET)){
            val = val < pp_bitShift();
        } else if(pp_consume(TK_L_ANGLE_BRACKET_EQUAL)){
            val = val <= pp_bitShift();
        } else if(pp_consume(TK_R_ANGLE_BRACKET)){
            val = val > pp_bitShift();
        } else if(pp_consume(TK_R_ANGLE_BRACKET_EQUAL)){
            val = val >= pp_bitShift();
        } else {
            break;
        }
    }
    return val;
}

static int pp_bitShift(){
    int val = pp_add();
    while(1){
        if(pp_consume(TK_L_BITSHIFT)){
            val = val << pp_add();
        } else if(pp_consume(TK_R_BITSHIFT)){
            val = val >> pp_add();
        } else {
            break;
        }
    }
    return val;
}

static int pp_add(){
    int val = pp_mul();
    while(1){
        if(pp_consume(TK_PLUS)){
            val = val + pp_mul();
        } else if(pp_consume(TK_MINUS)){
            val = val - pp_mul();
        } else {
            break;
        }
    }
    return val;
}

static int pp_mul(){
    int val = pp_unary();
    while(1){
        if(pp_consume(TK_MUL)){
            val = val * pp_unary();
        } else if(pp_consume(TK_DIV)){
            val = val / pp_unary();
        } else if(pp_consume(TK_PERCENT)){
            val = val % pp_unary();
        } else {
            break;
        }
    }
    return val;
}

static int pp_unary(){
    if(pp_consume(TK_PLUS)){
        return pp_primary();
    } else if(pp_consume(TK_MINUS)){
        return -pp_primary();
    } else if(pp_consume(TK_NOT)){
        return !pp_primary();
    } else {
        return pp_primary();
    }
}

static int pp_primary(){
    if(pp_consume(TK_L_PAREN)){
        int val = pp_expr();
        pp_consume(TK_R_PAREN);
        return val;
    } else {
        Token* token = expr_token;
        expr_token = next_token(expr_token);
        return get_token_int(token);
    }
}

static bool pp_consume(TokenKind kind){
    // もうトークンがないときはすべてfalse
    if(!expr_token) return false;

    // TK_SPACEのスキップ
    if(expr_token->kind == TK_SPACE){
        while(expr_token->kind == TK_SPACE){
            expr_token = expr_token->next;
        }
    }

    if(expr_token->kind == kind){
        expr_token = expr_token->next;
        return true;
    }
    return false;
}

static int get_token_int(Token* token){
    if(token->kind == TK_NUM){
        return token->val;
    } else {
        error("not a number");
        return 0;
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
