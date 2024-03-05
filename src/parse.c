#include "mcc2.h"
#include "tokenizer.h"
#include "ident.h"
#include "error.h"
#include "type.h"
#include "utility.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
    program = ( function | declaration )*
    function = ident '(' (declspec ident)*'){' compound_stmt
    stmt = expr ';' |
            'return' expr ';' |
            'if(' expr ')' stmt ('else' stmt)? |
            'while(' expr ')' stmt |
            'for(' (expr | declspec ident ('=' assign) ? )? ';' expr? '; expr? )' stmt |
            '{' compound_stmt |
            'switch(' expr ')' stmt |
            'case' const_expr ':' |
            'default:' |
            'goto' ident ';'
    compound_stmt = stmt* | declaration* '}'
    declaration = declare '=' (expr)? ';'
    declare = declspec ident
    declspec = 'int' '*' * | 'char' '*' * | 'short' '*' * | 'struct' ident | 'union' ident
    expr = assign (',' assign)*
    assign = cond_expr ( '=' assign
                        | '+=' assign
                        | '-=' assign
                        | '*=' assign
                        | '/=' assign
                        | '%=' assign
                        | '<<' assign
                        | '>>' assign
                    )?
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
    unary = ('+' | '-' | '&' unary | '*' unary | 'sizeof' unary )? postfix
    postfix = primary ('[' expr ']')*
    primary = '(' expr ')' | num | ident | ident '()'
*/

static Node* switch_node = NULL;
static Token* token = NULL;

static void Program();
static void function();
static Node* stmt();
static Node* compound_stmt();
static Node* declaration();
static Ident* declare(Type* ty);
static Type* declspec();
static Member* struct_or_union_member();
static Type* struct_or_union_spec(bool is_union);
static Node* expr();
static Node* assign();
static Node* cond_expr();
static Node* logicOr();
static Node* logicAnd();
static Node* bitOr();
static Node* bitXor();
static Node* bitAnd();
static Node* equality();
static Node* relational();
static Node* bitShift();
static Node* add();
static Node* mul();
static Node* unary();
static Node* postfix();
static Node* primary();
static Node* const_expr();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_add(Node* lhs, Node* rhs);
static Node* new_node_sub(Node* lhs, Node* rhs);
static Node* new_node_div(Node* lhs, Node* rhs);
static Node* new_node_mul(Node* lhs, Node* rhs);
static Node* new_node_mod(Node* lhs, Node* rhs);
static Node* new_node_num(int num);
static Node* new_node_var(Ident* ident);
static Node* new_inc(Node* var);
static Node* new_dec(Node* var);
static bool is_function();

// ----------------------------------------
// トークン操作
void expect_token(TokenKind kind);
bool consume_token(TokenKind kind);
Token* consume_ident();
Token* expect_ident();
Token* consume_string_literal();
int expect_num();
bool is_eof();
bool is_type();
bool is_label();
Token* get_token();
void set_token(Token* tok);


Token unnamed_struct_token = {
    TK_IDENT,
    "__unnamed_struct",
    NULL,
    0,
    sizeof("__unnamed_struct"),
    NULL,
};

void parse(Token* tok){
    token = tok;
    Program();
    return;
}

void Program(){
    while(!is_eof()){
        if(is_function()){
            function();
        } else {
            Type* ty = declspec();
            Ident* ident = declare(ty);
            register_ident(ident);
            ident->kind = ID_GVAR;
            expect_token(TK_SEMICORON);
        }
    }

    return;
}

static void function(){
    Type* func_type = declspec();
    Token* tok = consume_ident();
    if(!tok){
        // 識別子がない場合は、関数宣言がない
        return;
    }

    // 前方宣言があるか検索
    Ident* func = find_ident(tok);
    bool has_forward_def = false;       // 前方宣言あるか？（パラメータ個数チェック用）
    if(!func){
        func = declare_ident(tok, ID_FUNC, func_type);
    } else {
        // ある場合は戻り値型がconflictしてないかチェック
        if(!equal_type(func_type, func->type)){
            error_at(tok, "conflict definition type.");
        }
        has_forward_def = true;
    }

    // ここでスコープインして、仮引数は関数スコープ内で宣言するため、ローカル変数と同等に扱える。
    scope_in();
    expect_token(TK_L_PAREN);

    if(!consume_token(TK_R_PAREN)){
        Parameter head = {};
        Parameter* cur = &head;
        do {
            if(consume_token(TK_DOT_DOT_DOT)){
                func->is_var_params = true;
                break;
            } else {
                Type* ty = declspec();
                Ident* ident = declare(ty);
                register_ident(ident);
                Parameter* param = calloc(1, sizeof(Parameter));
                param->ident = ident;
                cur = cur->next = param;
            }
        } while(consume_token(TK_COMMA));
        func->params = head.next;
        expect_token(TK_R_PAREN);
    }

    func->scope = get_current_scope();

    if(consume_token(TK_SEMICORON)){
        // ここでセミコロンがあるなら前方宣言
        scope_out();
        return;
    } else {
        if(has_forward_def && func->funcbody){
            // すでに定義された関数のbodyがあるのに、ここでも定義している
            error_at(tok, "Conflict function definition.");
        }
    }

    expect_token(TK_L_BRACKET);
    func->funcbody = compound_stmt();
    func->stack_size = get_stack_size();
    scope_out();
    add_type(func->funcbody);   

    return;
}

static Node* stmt(){
    Token* tok = get_token();
    if(consume_token(TK_RETURN)){
        Node* node = new_node(ND_RETURN, expr(), NULL);
        node->pos = tok;
        expect_token(TK_SEMICORON);
        return node;
    } else if(consume_token(TK_IF)){
        Node* node = new_node(ND_IF, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->then = stmt();

        // else側がある場合
        if(consume_token(TK_ELSE)){
            node->elif = stmt();
            node->kind = ND_IF_ELSE;
        }
        return node;
    } else if(consume_token(TK_WHILE)){
        Node* node = new_node(ND_WHILE, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->body = stmt();
        return node;
    } else if(consume_token(TK_DO)){
        Node* node = new_node(ND_DO_WHILE, NULL, NULL);
        node->pos = tok;
        node->body = stmt();
        expect_token(TK_WHILE);
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        expect_token(TK_SEMICORON);
        return node;
    } else if(consume_token(TK_FOR)){
        Node* node = new_node(ND_FOR, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);

        // 初期化式
        if(consume_token(TK_SEMICORON)){
            node->init = NULL;
        } else {
            if(is_type()){
                Type* ty = declspec();
                Ident* ident = declare(ty);
                register_ident(ident);
                if(consume_token(TK_ASSIGN)){
                    node->init = new_node(ND_ASSIGN, new_node_var(ident), assign());
                }
            } else {
                node->init = expr();
            }
            expect_token(TK_SEMICORON);
        }

        // 継続条件の式
        if(consume_token(TK_SEMICORON)){
            node->cond = new_node_num(1);
        } else {
            node->cond = expr();
            expect_token(TK_SEMICORON);
        }

        // イテレートの式
        if(consume_token(TK_R_PAREN)){
            node->incr = NULL;
        } else {
            node->incr = expr();
            expect_token(TK_R_PAREN);
        }
        node->body = stmt();
        return node;
    } else if(consume_token(TK_L_BRACKET)){
        return compound_stmt();
    } else if(consume_token(TK_SEMICORON)){
        Node* node = new_node(ND_VOID_STMT, NULL, NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_BREAK)){
        Node* node = new_node(ND_BREAK, NULL, NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_CONTINUE)){
        Node* node = new_node(ND_CONTINUE, NULL, NULL);
        node->pos = tok;
        return node;
    } else if(consume_token(TK_SWITCH)){
        Node* node = new_node(ND_SWITCH, NULL, NULL);
        Node* switch_back = switch_node;
        switch_node = node;

        node->pos = tok;
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->body = stmt();

        switch_node = switch_back;
        return node;
    } else if(consume_token(TK_CASE)){
        if(switch_node == NULL){
            error_at(tok, "case is not within a switch statement.");
        }

        Node* node = new_node(ND_CASE, NULL, NULL);
        node->lhs = const_expr(); 
        consume_token(TK_COLON);
        node->pos = tok;

        node->next_case = switch_node->next_case;
        switch_node->next_case = node;
        return node;
    } else if(consume_token(TK_DEFAULT)){
        Node* node = new_node(ND_DEFAULT, NULL, NULL);
        node->pos = tok;
        consume_token(TK_COLON);

        if(switch_node->default_label){
            error_at(tok, "multiple default label.");
        }
        switch_node->default_label = node;
        return node;
    } else if(is_label()){
        Node* node = new_node(ND_LABEL, NULL, NULL);
        node->pos = tok;
        Token* label_ident = expect_ident();
        expect_token(TK_COLON);

        Label* label = find_label(label_ident);
        if(!label){
            label = register_label(label_ident);
        }
        label->labeld = true;
        node->label = label;
        return  node;
    } else if(consume_token(TK_GOTO)){
        Node* node = new_node(ND_GOTO, NULL, NULL);
        node->pos = tok;
        Token* label_ident = expect_ident();
        Label* label = find_label(label_ident);
        if(!label){
            label = register_label(label_ident);
        }
        node->label = label;
        return  node;
    } else {
        Node* node = expr();
        expect_token(TK_SEMICORON);
        node->pos = tok;
        return node;
    }
}

static Node* compound_stmt(){
    Node* node = new_node(ND_BLOCK, NULL, NULL);

    Node head = {};
    Node* cur = &head;
    while(!consume_token(TK_R_BRACKET)){
        if(is_type()){
            Node* node = declaration();
            if(node){
                cur = cur->next = node;
            }
        } else {
            cur->next = stmt();
            cur = cur->next;
        }
        add_type(cur);
    }
    node->body = head.next;
    return node;
}

static Node* declaration(){
    Type* ty = declspec();

    if(ty->kind == TY_STRUCT && consume_token(TK_SEMICORON)){
        // 構造体の登録を行う
        return new_node(ND_VOID_STMT, NULL, NULL);
    }

    if(ty->kind == TY_UMION && consume_token(TK_SEMICORON)){
        // 共用体の登録を行う
        return new_node(ND_VOID_STMT, NULL, NULL);
    }

    Ident* ident = declare(ty);
    register_ident(ident);

    Node* node = NULL;
    if(consume_token(TK_ASSIGN)){
        node = new_node(ND_ASSIGN, new_node_var(ident), assign());
    }

    expect_token(TK_SEMICORON);
    return node;
}

static Ident* declare(Type* ty){

    Token* ident_tok = expect_ident();
    if(consume_token(TK_L_SQUARE_BRACKET)){
        int len = expect_num();
        ty = array_of(ty, len);
        expect_token(TK_R_SQUARE_BRACKET);
    }

    return make_ident(ident_tok, ID_LVAR, ty);
}

static Type* declspec(){
    Type* ty;
    if(consume_token(TK_INT)){
        ty = ty_int;
    } else if(consume_token(TK_CHAR)){
        ty = ty_char;
    } else if(consume_token(TK_SHORT)){
        ty = ty_short;
    } else if(consume_token(TK_STRUCT)){
        ty = struct_or_union_spec(false);
    } else if(consume_token(TK_UNION)){
        ty = struct_or_union_spec(true);
    } else {
        fprintf(stderr, "%d\n", get_token()->kind);
        unreachable();
    }

    while(consume_token(TK_MUL)){
        ty = pointer_to(ty);
    }
    return ty;
}

static Member* struct_or_union_member(){
    Member head;
    Member* cur = &head;
    do {
        cur = cur->next = calloc(1, sizeof(Member));
        Type* ty = declspec();
        cur->ident = declare(ty);
        expect_token(TK_SEMICORON);
    } while(!consume_token(TK_R_BRACKET));
    return head.next;
}

static Type* struct_or_union_spec(bool is_union){
    Token* tok = consume_ident();
    if(tok){
        // 名前付き構造体
        Type* ty = find_struct_or_union_type(tok);

        if(!ty){
            // まだ登録されていない構造体
            ty = new_type(is_union ? TY_UMION : TY_STRUCT, 0);
            expect_token(TK_L_BRACKET);
            ty->member = struct_or_union_member();

            if(is_union){
                int max_size = 0;
                for(Member* cur = ty->member; cur; cur = cur->next){
                    cur->ident->offset = 0;
                    if(max_size < cur->ident->type->size){
                        max_size = cur->ident->type->size;
                    }
                }
                ty->size = max_size;
            } else {
                int offset = 0;
                for(Member* cur = ty->member; cur; cur = cur->next){
                    cur->ident->offset = offset;
                    offset += cur->ident->type->size;
                }
                ty->size = offset;
            }
            ty->name = tok;
            register_struct_or_union_type(ty);
        }
        return ty;
    } else {
        // 無名構造体
        Type* ty = new_type(is_union ? TY_UMION : TY_STRUCT, 0);
        expect_token(TK_L_BRACKET);
        ty->member = struct_or_union_member();

            if(is_union){
                int max_size = 0;
                for(Member* cur = ty->member; cur; cur = cur->next){
                    cur->ident->offset = 0;
                    if(max_size < cur->ident->type->size){
                        max_size = cur->ident->type->size;
                    }
                }
                ty->size = max_size;
            } else {
                int offset = 0;
                for(Member* cur = ty->member; cur; cur = cur->next){
                    cur->ident->offset = offset;
                    offset += cur->ident->type->size;
                }
                ty->size = offset;
            }
        ty->name = &unnamed_struct_token;
        return ty;
    }
}

static Node* expr(){
    Node* node = assign();
    while(consume_token(TK_COMMA)){
        node = new_node(ND_COMMA, node, assign());
    }
    return node;
}

static Node* assign(){
    Node* node = cond_expr();
    if(consume_token(TK_ASSIGN)){
        node = new_node(ND_ASSIGN, node, assign());
    } else if(consume_token(TK_PLUS_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_add(node, assign()));
    } else if(consume_token(TK_MINUS_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_sub(node, assign()));
    } else if(consume_token(TK_MUL_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_mul(node, assign()));
    } else if(consume_token(TK_DIV_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_div(node, assign()));
    } else if(consume_token(TK_PERCENT_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node_mod(node, assign()));
    } else if(consume_token(TK_L_BITSHIFT_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node(ND_L_BITSHIFT, node, assign()));
    } else if(consume_token(TK_R_BITSHIFT_EQUAL)){
        node = new_node(ND_ASSIGN, node, new_node(ND_R_BITSHIFT, node, assign()));
    }
    return node;
}

static Node* cond_expr(){
    Node* node = logicOr();

    if(consume_token(TK_QUESTION)){
        Node* cnode = new_node(ND_COND_EXPR, NULL, NULL);
        cnode->cond = node;
        cnode->lhs = expr();
        expect_token(TK_COLON);
        cnode->rhs = cond_expr();
        node = cnode;
    }
    return node;
}

static Node* logicOr(){
    Node* node = logicAnd();

    while(true){
        if(consume_token(TK_PIPE_PIPE)){
            node = new_node(ND_LOGIC_OR, node, logicAnd());
        } else {
            return node;
        }
    }
}

static Node* logicAnd(){
    Node* node = bitOr();

    while(true){
        if(consume_token(TK_AND_AND)){
            node = new_node(ND_LOGIC_AND, node, bitOr());
        } else {
            return node;
        }
    }
}

static Node* bitOr(){
    Node* node = bitXor();

    while(true){
        if(consume_token(TK_PIPE)){
            node = new_node(ND_BIT_OR, node, bitXor());
        } else {
            return node;
        }
    }
}

static Node* bitXor(){
    Node* node = bitAnd();

    while(true){
        if(consume_token(TK_HAT)){
            node = new_node(ND_BIT_XOR, node, bitAnd());
        } else {
            return node;
        }
    }
}

static Node* bitAnd(){
    Node* node = equality();

    while(true){
        if(consume_token(TK_AND)){
            node = new_node(ND_BIT_AND, node, equality());
        } else {
            return node;
        }
    }
}

static Node* equality(){
    Node* node = relational();

    while(true){
        if(consume_token(TK_EQUAL)){
            node = new_node(ND_EQUAL, node, relational());
        } else if(consume_token(TK_NOT_EQUAL)){
            node = new_node(ND_NOT_EQUAL, node, relational());
        } else {
            return node;
        }
    }
}

static Node* relational(){
    Node* node = bitShift();
    while(true){
        if(consume_token(TK_L_ANGLE_BRACKET)){
            node = new_node(ND_LT, node, bitShift());
        } else if(consume_token(TK_L_ANGLE_BRACKET_EQUAL)){
            node = new_node(ND_LE, node, bitShift());
        } else if(consume_token(TK_R_ANGLE_BRACKET)){
            node = new_node(ND_LT, bitShift(), node);
        } else if(consume_token(TK_R_ANGLE_BRACKET_EQUAL)){
            node = new_node(ND_LE, bitShift(), node);
        } else {
            return node;
        }
    }
}

static Node* bitShift(){
    Node* node = add();

    while(true) {
        if(consume_token(TK_L_BITSHIFT)){
            node = new_node(ND_L_BITSHIFT, node, add());
        } else if(consume_token(TK_R_BITSHIFT)){
            node = new_node(ND_R_BITSHIFT, node, add());
        } else {
            return node;
        }
    }
}

static Node* add(){
    Node* node = mul();

    while(true){
        if(consume_token(TK_PLUS)){
            node = new_node(ND_ADD, node, mul());
        } else if(consume_token(TK_MINUS)){
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

static Node* mul(){
    Node* node = unary();

    while(true){
        if(consume_token(TK_MUL)){
            node = new_node(ND_MUL, node, unary());
        } else if(consume_token(TK_DIV)){
            node = new_node(ND_DIV, node, unary());
        } else if(consume_token(TK_PERCENT)){
            node = new_node(ND_MOD, node, unary());
        } else {
            break;
        }
    }

    return node;
}

static Node* unary(){
    if(consume_token(TK_PLUS)){
        return unary();
    } else if(consume_token(TK_MINUS)){
        return new_node_sub(new_node_num(0), unary());
    } else if(consume_token(TK_AND)){
        return new_node(ND_ADDR, unary(), NULL);
    } else if(consume_token(TK_MUL)){
        return new_node(ND_DREF, unary(), NULL);
    } else if(consume_token(TK_PLUS_PLUS)){
        Node* node = unary();
        return new_node(ND_ASSIGN, node, new_node_add(node, new_node_num(1)));
    } else if(consume_token(TK_MINUS_MINUS)){
        Node* node = unary();
        return new_node(ND_ASSIGN, node, new_node_sub(node, new_node_num(1)));
    } else if(consume_token(TK_SIZEOF)){
        Node* node = unary();
        add_type(node);
        if(node->type->kind == TY_ARRAY){
            return new_node_num(node->type->array_len * node->type->size);
        } else {
            return new_node_num(node->type->size);
        }
    }
    return postfix();
}

static Node* postfix(){
    Node* node = primary();

    while(true){
        if(consume_token(TK_L_SQUARE_BRACKET)){
            node = new_node(ND_DREF, new_node_add(node, expr()), NULL);
            expect_token(TK_R_SQUARE_BRACKET);
            continue;
        }

        if(consume_token(TK_PLUS_PLUS)){
            node = new_inc(node);
            continue;
        }

        if(consume_token(TK_MINUS_MINUS)){
            node = new_dec(node);
            continue;
        }

        if(consume_token(TK_DOT)){
            add_type(node);

            node = new_node(ND_MEMBER, node, NULL);

            // find a member
            Token* tok = expect_ident();
            Ident* ident = get_member(node->lhs->type, tok);
            if(!ident){
                error_at(tok, "Not a member.\n");
            }

            node->type = ident->type;
            node->val = ident->offset;
            continue;
        }

        return node;
    }
}

static Node* primary(){
    if(consume_token(TK_L_PAREN)){
        Node* node = expr();
        expect_token(TK_R_PAREN);
        return node;
    }

    Token* tok_str = consume_string_literal();
    if(tok_str){
        Ident* str_ident = register_string_literal(tok_str);
        Node* node = new_node(ND_VAR, 0, 0);
        node->ident = str_ident;
        node->type = str_ident->type;
        return node;
    }

    Token* ident_token = consume_ident();
    if(ident_token){
        Ident* ident = find_ident(ident_token);
        if(!ident){
            error_at(ident_token, "Undefined Variable.");
        } else {
            if((ident->kind == ID_LVAR) || (ident->kind == ID_GVAR)){
                Node* node = new_node(ND_VAR, 0, 0);
                node->ident = ident;
                node->type = ident->type;
                return node;
            } else if(ident->kind == ID_FUNC){
                if(consume_token(TK_L_PAREN)){
                    Node* node = new_node(ND_FUNCCALL, 0, 0);
                    node->ident = ident;
                    node->type = ident->type;
                    if(!consume_token(TK_R_PAREN)){
                        Node head = {};
                        Node* nd_param = &head;
                        do{
                            nd_param = nd_param->next = assign();
                        } while(consume_token(TK_COMMA));
                        expect_token(TK_R_PAREN);
                        node->params = head.next;
                    }
                    return node;
                }
                unreachable();
            }
        }
    }

    return new_node_num(expect_num());
}

static Node* const_expr(){
    Token* tok = get_token();
    Node* node = expr();
    if(node->kind != ND_NUM){
        error_at(tok, "expected constant expression.");
    }
    return node;
}

static Node* new_node(NodeKind kind, Node* lhs, Node* rhs){
    Node* result = calloc(1, sizeof(Node));
    result->kind = kind;
    result->lhs = lhs;
    result->rhs = rhs;
    return result;
}

static Node* new_node_num(int num){
    Node* result = calloc(1, sizeof(Node));
    result->val = num;
    return result;
}

static Node* new_node_var(Ident* ident){
    Node* result = calloc(1, sizeof(Node));
    result->kind = ND_VAR;
    result->ident = ident;

    return result;
}

static Node* new_node_add(Node* lhs, Node* rhs){

    add_type(lhs);
    add_type(rhs);

    Node* result = new_node(ND_ADD, lhs, rhs);

    // num + num
    if(!lhs->type->ptr_to && !rhs->type->ptr_to)
    {
        return result;
    }
    
    // pointer + pointer
    if(lhs->type->ptr_to && rhs->type->ptr_to){
        error("tTry add pointer and pointer.");
    }
    
    // num + pointer
    if(!lhs->type->ptr_to && rhs->type->ptr_to){
        // ポインタ + 数値、になるように入れ替える
        Node* buf = lhs;
        rhs = lhs;
        lhs = buf;
    }

    Node* node_mul = new_node_mul(rhs, new_node_num(lhs->type->ptr_to->size));
    result->lhs = lhs;
    result->rhs = node_mul;

    return result;
}
static Node* new_node_sub(Node* lhs, Node* rhs){

    add_type(lhs);
    add_type(rhs);

    Node* result = new_node(ND_SUB, lhs, rhs);

    // num + num
    if(!lhs->type->ptr_to && !rhs->type->ptr_to)
    {
        return result;
    }
    
    // - pointer
    if(rhs->type->ptr_to){
        error("invalid operand.");
    }
    
    // pointer - num
    if(lhs->type->ptr_to && !rhs->type->ptr_to){
        Node* node_mul = new_node_mul(rhs, new_node_num(lhs->type->ptr_to->size));
        result->lhs = lhs;
        result->rhs = node_mul;
        return result;
    }
}
static Node* new_node_div(Node* lhs, Node* rhs){
    Node* result = new_node(ND_DIV, lhs, rhs);
    return result;
}
static Node* new_node_mul(Node* lhs, Node* rhs){
    Node* result = new_node(ND_MUL, lhs, rhs);
    return result;
}
static Node* new_node_mod(Node* lhs, Node* rhs){
    Node* result = new_node(ND_MOD, lhs, rhs);
    return result;
}

static Node* new_inc(Node* var){
    Token tok;
    tok.pos = "tmp";
    tok.len = 3;
    tok.kind = TK_IDENT;
    scope_in();
    Type* ty = calloc(1, sizeof(Type));
    memcpy(ty, var->type, sizeof(Type));

    Ident* tmp = declare_ident(&tok, ID_LVAR, ty);

    Node* node_tmp = new_node_var(tmp);
    Node* node_assign = new_node(ND_ASSIGN, node_tmp, var);
    Node* node_inc = new_node(ND_ASSIGN, var, new_node_add(var, new_node_num(1)));
    Node* node = new_node(ND_COMMA, node_assign, new_node(ND_COMMA, node_inc, node_tmp));

    scope_out();
    return node;
}

static Node* new_dec(Node* var){
    Token tok;
    tok.pos = "tmp";
    tok.len = 3;
    tok.kind = TK_IDENT;
    scope_in();
    Type* ty = calloc(1, sizeof(Type));
    memcpy(ty, var->type, sizeof(Type));

    Ident* tmp = declare_ident(&tok, ID_LVAR, ty);

    Node* node_tmp = new_node_var(tmp);
    Node* node_assign = new_node(ND_ASSIGN, node_tmp, var);
    Node* node_inc = new_node(ND_ASSIGN, var, new_node_sub(var, new_node_num(1)));
    Node* node = new_node(ND_COMMA, node_assign, new_node(ND_COMMA, node_inc, node_tmp));

    scope_out();
    return node;
}

static bool is_function(){
    Token* bkup = get_token();
    bool retval = false;

    // 型の読み取り
    Type* ty = declspec();

    // ポインタの読み取り
    while(consume_token(TK_MUL)){
        ;
    }

    expect_token(TK_IDENT);
    if(consume_token(TK_L_PAREN)){
        retval = true;
    }

    set_token(bkup);
    return retval;
}

// ----------------------------
// トークン操作
void expect_token(TokenKind kind){
    if(token->kind != kind){
        error_at(token, "error: unexpected token.\n");
    }

    token = token->next;
}

int expect_num(){
    if(token->kind != TK_NUM){
        error_at(token, "error: not a number.\n", token->pos);
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

Token* consume_string_literal(){
    if(token->kind != TK_STRING_LITERAL) return NULL;
    Token* tok = token;
    token = token->next;
    return tok;
}

Token* expect_ident(){
    Token* tok = consume_ident();
    if(tok == NULL){
        error_at(token, "error: not a ident.\n", token->pos);
    }
    return tok;
}

bool is_eof(){
    return token->kind == TK_EOF;
}


bool is_type(){
    return token->kind == TK_STRUCT
        || token->kind == TK_UNION
        || token->kind == TK_INT
        || token->kind == TK_SHORT
        || token->kind == TK_CHAR;
}

bool is_label(){
    if(token->kind == TK_IDENT && token->next->kind == TK_COLON){
        return true;
    }
    return false;
}

Token* get_token(){
    return token;
}

void set_token(Token* tok){
    token = tok;
}
