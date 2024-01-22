#include "mcc2.h"
#include "tokenizer.h"
#include "ident.h"
#include <stdbool.h>
#include <stdlib.h>

/*
    program = function*
    function = ident '(){' stmt* '}'
    stmt = expr ';' |
            'return' expr ';' |
            'if(' expr ')' stmt ('else' stmt)? |
            'while(' expr ')' stmt | 
            'for(' expr ';' expr ';)' stmt |
            '{' compound_stmt
    compound_stmt = stmt* '}'
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
    unary = ('+' | '-')? primary
    primary = '(' expr ')' | num | ident | ident '()'
*/

static Function* function();
static Node* stmt();
static Node* compound_stmt();
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
static Node* primary();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_add(Node* lhs, Node* rhs);
static Node* new_node_sub(Node* lhs, Node* rhs);
static Node* new_node_div(Node* lhs, Node* rhs);
static Node* new_node_mul(Node* lhs, Node* rhs);
static Node* new_node_mod(Node* lhs, Node* rhs);
static Node* new_node_num(int num);
static Node* new_node_lvar(Ident* ident);

Function* Program(){
    Function head;
    Function* cur = &head;

    while(!is_eof()){
        cur->next = function();
        cur = cur->next;
    }

    return head.next;
}

static Function* function(){
    Function* func = calloc(1, sizeof(Function));
    
    Token* tok = consume_ident();
    if(!tok){
        // 識別子がない場合は、関数宣言がない
        return NULL;
    }

    func->name = declare_ident(tok, 0, ID_FUNC);
    scope_in();
    expect_token(TK_L_PAREN);

    if(!consume_token(TK_R_PAREN)){
        Parameter head = {};
        Parameter* cur = &head;
        do {
            Token* tok = expect_ident();
            Ident* ident = declare_ident(tok, 8, ID_LVAR);
            Parameter* param = calloc(1, sizeof(Parameter));
            param->ident = ident;
            cur = cur->next = param;
        } while(consume_token(TK_CANMA));
        func->params = head.next;
        expect_token(TK_R_PAREN);
    }

    expect_token(TK_L_BRACKET);
    func->stmts = compound_stmt();
    func->stack_size = get_stack_size();
    scope_out();

    return func;
}

static Node* stmt(){
    if(consume_token(TK_RETURN)){
        Node* node = new_node(ND_RETURN, expr(), NULL);
        expect_token(TK_SEMICORON);
        return node;
    } else if(consume_token(TK_IF)){
        Node* node = new_node(ND_IF, NULL, NULL);
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
        expect_token(TK_L_PAREN);
        node->cond = expr();
        expect_token(TK_R_PAREN);
        node->body = stmt();
        return node;
    } else if(consume_token(TK_FOR)){
        Node* node = new_node(ND_FOR, NULL, NULL);
        expect_token(TK_L_PAREN);
        node->init = expr();
        expect_token(TK_SEMICORON);
        node->cond = expr();
        expect_token(TK_SEMICORON);
        node->incr = expr();
        expect_token(TK_R_PAREN);
        node->body = stmt();
        return node;
    } else if(consume_token(TK_L_BRACKET)){
        return compound_stmt();
    } else {
        Node* node = expr();
        expect_token(TK_SEMICORON);
        return node;        
    }
}

static Node* compound_stmt(){
    Node* node = new_node(ND_BLOCK, NULL, NULL);

    Node head = {};
    Node* cur = &head;
    while(!consume_token(TK_R_BRACKET)){
        cur->next = stmt();
        cur = cur->next;
    }
    node->body = head.next;
    return node;
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
    }
    return primary();
}

static Node* primary(){
    if(consume_token(TK_L_PAREN)){
        Node* node = expr();
        expect_token(TK_R_PAREN);
        return node;
    }

    Token* ident_token = consume_ident();
    if(ident_token){
        NodeKind kind = ND_LVAR;
        if(consume_token(TK_L_PAREN)){
            // 関数の場合
            expect_token(TK_R_PAREN);
            kind = ND_FUNCCALL;
        }
        // 変数の場合
        Ident* ident = find_ident(ident_token);
        if(!ident){
            ident = declare_ident(ident_token, 8, ID_LVAR);
        }

        Node* node = new_node(kind, 0, 0);
        node->ident = ident;
        return node;
    }

    return new_node_num(expect_num());
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

static Node* new_node_lvar(Ident* ident){
    Node* result = calloc(1, sizeof(Node));
    result->kind = ND_LVAR;
    result->ident = ident;

    return result;
}

static Node* new_node_add(Node* lhs, Node* rhs){
    Node* result = new_node(ND_ADD, lhs, rhs);
    return result;
}
static Node* new_node_sub(Node* lhs, Node* rhs){
    Node* result = new_node(ND_SUB, lhs, rhs);
    return result;
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