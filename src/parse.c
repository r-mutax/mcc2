#include "mcc2.h"
#include "tokenizer.h"
#include "ident.h"
#include "error.h"
#include "type.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

/*
    program = ( function | declaration )*
    function = ident '(' (declspec ident)*'){' compound_stmt
    stmt = expr ';' |
            'return' expr ';' |
            'if(' expr ')' stmt ('else' stmt)? |
            'while(' expr ')' stmt | 
            'for(' (expr | declspec ident ('=' assign) ? )? ';' expr? '; expr? )' stmt |
            '{' compound_stmt
    compound_stmt = stmt* | declaration* '}'
    declaration = declare '=' (expr)? ';'
    declare = declspec ident
    declspec = 'int' '*' *
    expr = assign
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

static void function();
static Node* stmt();
static Node* compound_stmt();
static Node* declaration();
static Ident* declare();
static Type* declspec();
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
static bool is_function();

void Program(){
    while(!is_eof()){
        if(is_function()){
            function();
        } else {
            Ident* ident = declare();
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
            error_at(tok->pos, "conflict definition type.");
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
                Ident* ident = declare();
                Parameter* param = calloc(1, sizeof(Parameter));
                param->ident = ident;
                cur = cur->next = param;
            }
        } while(consume_token(TK_CANMA));
        func->params = head.next;
        expect_token(TK_R_PAREN);
    }

    if(consume_token(TK_SEMICORON)){
        // ここでセミコロンがあるなら前方宣言
        scope_out();
        return;
    } else {
        if(has_forward_def && func->funcbody){
            // すでに定義された関数のbodyがあるのに、ここでも定義している
            error_at(tok->pos, "Conflict function definition.");
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
    } else if(consume_token(TK_FOR)){
        Node* node = new_node(ND_FOR, NULL, NULL);
        node->pos = tok;
        expect_token(TK_L_PAREN);

        // 初期化式
        if(consume_token(TK_SEMICORON)){
           node->init = NULL; 
        } else {
            if(is_type()){
                Ident* ident = declare();
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
            error_at(tok->pos, "case is not within a switch statement.");
        }

        Node* node = new_node(ND_CASE, NULL, NULL);
        node->lhs = const_expr(); 
        consume_token(TK_CORON);
        node->pos = tok;

        node->next_case = switch_node->next_case;
        switch_node->next_case = node;
        return node;
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
    Ident* ident = declare();

    Node* node = NULL;
    if(consume_token(TK_ASSIGN)){
        node = new_node(ND_ASSIGN, new_node_var(ident), assign());
    }

    expect_token(TK_SEMICORON);
    return node;
}

static Ident* declare(){
    Type* ty = declspec();
    Token* ident_tok = expect_ident();
    if(consume_token(TK_L_SQUARE_BRACKET)){
        int len = expect_num();
        ty = array_of(ty, len);
        expect_token(TK_R_SQUARE_BRACKET);
    }

    Ident* ident = declare_ident(ident_tok, ID_LVAR, ty);
    return ident;
}

static Type* declspec(){
    Type* ty;
    if(consume_token(TK_INT)){
        ty = ty_int;
    } else if(consume_token(TK_CHAR)){
        ty = ty_char;
    } else if(consume_token(TK_SHORT)){
        ty = ty_short;
    } else {
        unreachable();
    }

    while(consume_token(TK_MUL)){
        ty = pointer_to(ty);
    }
    return ty;
}

static Node* expr(){
    return assign();
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
        expect_token(TK_CORON);
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
            error_at(ident_token->pos, "Undefined Variable.");
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
                        } while(consume_token(TK_CANMA));
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
        error_at(tok->pos, "expected constant expression.");
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