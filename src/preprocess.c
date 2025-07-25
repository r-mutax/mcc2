#include "mcc2.h"

IncludePath* include_paths = NULL;
IncludePath* std_include_paths = NULL;
extern bool is_preprocess;
extern char* PRE_MACRO[];

// pragma once されたヘッダのパスリスト
char* once_header_paths[1024];
int once_header_paths_cnt = 0;

Token tok_zero = {
    .kind = TK_NUM,
    .val = 0,
};

Token tok_one = {
    .kind = TK_NUM,
    .val = 1,
};

Token tok_place_holder = {
    .kind = TK_PLACE_HOLDER,
};

Macro* macros = NULL;

static char* find_include_file(char* filename);
static char* find_include_next_file(char* filename);

static void add_macro(Token* target);
static void add_macro_objlike(Token* target);
static void add_macro_funclike(Token* target);
static void delete_macro(Macro* m);
static Macro* find_macro(Token* name, Macro* mac);
static Token* delete_space_placeholder(Token* token);
static Token* replace_token(Token* tok, Macro* mac, Macro* list);
static Macro* copy_macro(Macro* mac);
static bool is_expand(Token* tok, Macro* list);

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

// pragma
static int check_pragma_once_include(char* path);

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

void init_preprocess(){
    for(int i = 0; i < 2; i++){
        tokenize_string(PRE_MACRO[i]);
    }
}

Token* preprocess(Token* token){
    Token head = {};
    head.next = token;
    Token* cur = &head;
    while(cur->next){
        Token* t1 = cur->next;
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

                        Token* newline = next_newline(target);
                        if(!check_pragma_once_include(path))
                        {
                            Token* include = tokenize(path);

                            // #ifdefの結果などから、読み込んだヘッダが空（==TK_EOFのみ）となることがあるが、
                            // そいういうときは、includeのトークンリストは繋げない
                            if(include->kind != TK_EOF){
                                // get tail of tokenlist from include
                                Token* tail = include;
                                while(next_token(tail)){
                                    if(next_token(tail)->kind != TK_EOF){
                                        tail = next_token(tail);
                                    } else {
                                        break;
                                    }
                                }
                                tail->next = next_token(newline);
                                cur->next = include;
                            } else {
                                cur->next = next_token(newline);
                            }
                        } else {
                            cur->next = next_token(newline);
                        }
                        continue;
                    }
                    break;
                case TK_INCLUDE_NEXT:
                    {
                        char* filepath = get_token_string(next_token(target));
                        char* path = find_include_next_file(filepath);
                        if(!path){
                            error("file not found: %s", filepath);
                        }

                        Token* newline = next_newline(target);
                        if(!check_pragma_once_include(path))
                        {
                            Token* include = tokenize(path);
                            if(include->kind == TK_EOF){
                                Token* tail = include;
                                while(next_token(tail)){
                                    if(next_token(tail)->kind != TK_EOF){
                                        tail = next_token(tail);
                                    } else {
                                        break;
                                    }
                                }
                                tail->next = next_token(newline);
                                cur->next = include;
                            } else {
                                cur->next = next_token(newline);
                            }
                        } else {
                            cur->next = next_token(newline);
                        }
                        continue;
                    }
                case TK_DEFINE:
                        add_macro(target);
                        cur->next = next_newline(target);
                        continue;
                    break;
                case TK_UNDEF:
                    {
                        Token* undefsymbol = next_token(target);
                        Macro* m = find_macro(undefsymbol, macros);
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
                case TK_PRAGMA:
                    {
                        Token* command = next_token(target);
                        if(command->kind == TK_IDENT){
                            if(strcmp(get_token_string(command), "once") == 0){
                                if(once_header_paths_cnt >= 1024){
                                    error("too many once header paths");
                                }
                                once_header_paths[once_header_paths_cnt++] = target->file->path;
                            }
                        }
                    }
                    cur->next = next_newline(target);
                    break;
                case TK_ERROR:
                    {
                        Token* msg = next_token(target);
                        if(msg->kind != TK_NEWLINE){
                            error_at_src(msg->pos, msg->file, "preprocessor error: %s", get_token_string(msg));
                        } else {
                            error_at_src(msg->pos, msg->file, "preprocessor error");
                        }
                    }
                    cur->next = next_newline(target);
                    break;
                default:
                    break;
            }
        } else if(t1->kind == TK_IDENT){
            Token* ident = t1;
            Macro* m = find_macro(ident, macros);
            if(m){
                cur->next = replace_token(ident, m, NULL);
            }
        }

        cur = cur->next;
    }

    if(!is_preprocess)
        head.next = delete_space_placeholder(head.next);

    // 隣接するstring literalを連結する
    Token* cur2 = head.next;
    while(cur2){
        if(cur2->kind == TK_STRING_LITERAL){
            Token* next = next_token(cur2);
            if(next && next->kind == TK_STRING_LITERAL){
                cur2->len += next->len;
                cur2->str = realloc(cur2->str, cur2->len);
                memcpy(cur2->str + cur2->len - next->len, next->str, next->len);
                cur2->next = next->next;
                continue;
            }
        }
        cur2 = cur2->next;
    }

    return head.next;
}

void add_include_path(char* path){
    IncludePath* p = calloc(1, sizeof(IncludePath));
    p->path = path;
    p->next = include_paths;
    include_paths = p;
}

void add_predefine_macro(char* name){
    Token* tok = calloc(1, sizeof(Token));
    tok->pos = strnewcpyn(name, strlen(name));
    tok->len = strlen(name);
    tok->kind = TK_IDENT;

    Macro* m = calloc(1, sizeof(Macro));
    m->name = tok;
    m->value = NULL;
    m->next = macros;
    macros = m;
}

static Token* delete_space_placeholder(Token* token){
    Token head = {};
    head.next = token;
    Token* cur = &head;

    while(cur->next){
        Token* target = cur->next;
        if(target->kind == TK_SPACE){
            cur->next = target->next;
        } else if(target->kind == TK_PLACE_HOLDER){
            cur->next = target->next;
        } else if(target->kind == TK_NEWLINE){
            cur->next = target->next;
        } else {
            cur = cur->next;
        }
    }

    return head.next;

}

static Macro* copy_macro(Macro* mac){
    Macro* ret = calloc(1, sizeof(Macro));
    memcpy(ret, mac, sizeof(Macro));
    ret->next = NULL;

    return ret;
}

static bool is_expand(Token* tok, Macro* list){
    Macro* cur = list;
    while(cur){
        if(is_equal_token(tok, cur->name)){
            return true;
        }
        cur = cur->next;
    }

    return false;
}

static Macro* make_param_list(Token* tok, Macro* mac){
    // トークンを引数まで進める
    while(tok->kind != TK_L_PAREN){
        tok = next_token(tok);
    }
    tok = next_token(tok);

    Macro head;
    Macro* cur = &head;
    Token* param = mac->params;

    int paren_cnt = 1;

    while(true){
        Macro* m = calloc(1, sizeof(Macro));
        m->name = copy_token(param);
        Token th = {};
        Token* tc = &th;
        int paren_cnt = 0;
        while(!(paren_cnt == 0 && (tok->kind == TK_COMMA || tok->kind == TK_R_PAREN))){
            if(tok->kind == TK_L_PAREN){
                paren_cnt++;
            } else if(tok->kind == TK_R_PAREN){
                paren_cnt--;
            }
            tc = tc->next = copy_token(tok);
            tok = next_token(tok);
        }
        m->value = th.next;

        cur = cur->next = m;

        // カンマならトークンを進めて次のパラメータへ、右括弧なら終了、それ以外はエラー
        if(tok->kind == TK_COMMA){
            tok = next_token(tok);
            param = next_token(param);
        } else if(tok->kind == TK_R_PAREN){
            break;
        } else {
            error_tok(tok, "expected comma operator or right parenthesis.");
        }
    }
    return head.next;
}

// 関数マクロのパラメータ展開
static Token* expand_funclike_macro_parameter(Token* tok, Macro* mac){

    Macro* param = make_param_list(tok, mac);

    Token head;
    head.next = copy_token_list(mac->value);
    Token* cur = &head;
    while(cur->next){
        Token* target = cur->next;
        if(target->kind == TK_IDENT){
            Macro* m = find_macro(target, param);
            if(m){
                Token* val = copy_token_list(m->value);
                Token* tail = get_tokens_tail(val);
                tail->next = cur->next->next;
                cur->next = val;
            }
        }
        cur = cur->next;
    }
    return head.next;
}

static Token* replace_token(Token* tok, Macro* mac, Macro* list){

    if(!list){
        list = copy_macro(mac);
    } else {
        Macro* buf = copy_macro(mac);
        buf->next = list;
        list = buf;
    }

    // 置き換え後のトークン取得
    Token* val = NULL;
    if(mac->is_func){
        val = expand_funclike_macro_parameter(tok, mac);
    } else {
        val = copy_token_list(mac->value);
    }

    if(!val) return tok->next;

    // マクロを再帰的に展開
    Token head = {};
    head.next = val;
    Token* cur = &head;
    Token* target = NULL;
    while(target = next_token(cur)){
        if(target->kind == TK_IDENT){
            if(is_expand(target, list)){
                cur = next_token(cur);
                continue;
            }

            Macro* m = find_macro(target, macros);
            if(m){
                cur->next = replace_token(target, m, list);
                cur = cur->next;
                continue;
            }
        }
        cur = next_token(cur);
    }

    Token* tail = get_tokens_tail(&head);
    if(mac->is_func){
        Token* t = skip_to_next(tok, TK_L_PAREN)->next;
        int paren_cnt = 0;
        while(!(paren_cnt == 0 && t->kind == TK_R_PAREN)){
            if(t->kind == TK_L_PAREN){
                paren_cnt++;
            } else if(t->kind == TK_R_PAREN){
                paren_cnt--;
            }
            t = t->next;
        }
        // TODO : エラー処理
        tail->next = t->next;
    } else {
        tail->next = tok->next;
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

static char* find_include_next_file(char* filename){

    int read_cnt = count_include_file(filename);
    if(read_cnt == 0){
        // 一度も読み込んでいないので、エラーとする
        error("include_next: file not found: %s", filename);
    }

    read_cnt++;
    IncludePath* p;
    int find_cnt = 0;
    for(p = include_paths; p; p = p->next){
        char* path = format_string("%s/%s", p->path, filename);
        if(fopen(path, "r")){
            find_cnt++;
            if(find_cnt == read_cnt){
                return path;
            }
        }
    }

    for(p = std_include_paths; p; p = p->next){
        char* path = format_string("%s/%s", p->path, filename);
        if(fopen(path, "r")){
            find_cnt++;
            if(find_cnt == read_cnt){
                return path;
            }
        }
    }

    return NULL;
}

static void add_macro_objlike(Token* target){
    Token* name = next_token(target);
    Token* value = next_token(name);

    Macro* m = calloc(1,sizeof(Macro));
    m->name = copy_token(name);
    if(value->kind != TK_NEWLINE){
        m->value = copy_token_eol(value);
    } else {
        m->value = copy_token(&tok_place_holder);
    }
    m->next = macros;
    macros = m;
}

static void add_macro_funclike(Token* target){
    Token* name = next_token(target);
    target = next_token(name);
    target = next_token(target);

    // 引数リストを作る
    Token head;
    Token* cur = &head;
    while(target->kind != TK_R_PAREN){
        if(target->kind != TK_IDENT){
            error_tok(target, "invalid macro argument. : code %d\n", target->kind);
        }

        cur = cur->next = copy_token(target);
        target = next_token(target);
        if(target->kind == TK_R_PAREN){
            break;
        }

        if(target->kind != TK_COMMA){
            error_tok(target, "expected comma operator");
        } else {
            target = next_token(target);
        }
    }

    target = next_token(target);

    Macro* m = calloc(1, sizeof(Macro));
    m->name = copy_token(name);
    m->params = head.next;
    if(target->kind != TK_NEWLINE){
        m->value = copy_token_eol(target);
    } else {
        m->value = copy_token(&tok_place_holder);
    }
    m->is_func = true;
    m->next = macros;
    macros = m;
}

static void add_macro(Token* target){
    Token* name = next_token(target);

    if(next_token(name)->kind == TK_L_PAREN){
        add_macro_funclike(target);
    } else {
        add_macro_objlike(target);
    }
}

static Macro* find_macro(Token* name, Macro* mac){
    if(name->kind != TK_IDENT){
        return NULL;
    }

    Macro* m;
    for(m = mac; m; m = m->next){
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
                default:
                    cur = cur->next;
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
                Macro* m = find_macro(next_token(token), macros);
                return m != NULL;
            }
            break;
        case TK_PP_IFNDEF:
            {
                Macro* m = find_macro(next_token(token), macros);
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

            cur->next = find_macro(ident, macros) ? &tok_one : &tok_zero;
            cur->next->next = target;
        } else {
            cur = next_token(cur);
        }
    }
    return head.next;
}

static Token* expand_macros(Token* token){
    Token head = {};
    Token* cur = &head;
    cur->next = token;

    while(cur && cur->next){
        Token* target = next_token(cur);
        if(target->kind == TK_IDENT){
            Macro* m = find_macro(target, macros);
            if(m){
                cur->next = replace_token(target, m, NULL);
            }
        }
        cur = next_token(cur);
    }

    return head.next;
}

static bool eval_expr(Token* token){
    token = expand_defined(token);
    token = expand_macros(token);
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

static int check_pragma_once_include(char* path){
    for(int i = 0; i < once_header_paths_cnt; i++){
        if(strcmp(path, once_header_paths[i]) == 0){
            return 1;
        }
    }
    return 0;
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
