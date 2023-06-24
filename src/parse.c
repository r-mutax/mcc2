#include "mcc2.h"
#include "tokenizer.h"
#include "ident.h"
#include <stdbool.h>
#include <stdlib.h>

/*
    program = stmt*
    stmt = expr ';' |
            'return' expr ';' |
            'if(' expr ')' stmt ('else' stmt)? 
    expr = assign
    assign = equality ( '=' assign )?
    equality = relational ('==' relational | '!=' relational)*
    relational = add ('<' add | '<=' add | '>' add | '>=' add)*
    add = mul ('+' mul | '-' mul)*
    mul = unary ('*' unary | '/' unary)
    unary = ('+' | '-')? primary
    primary = '(' expr ')' | num | ident
*/

static Node* stmt();
static Node* expr();
static Node* assign();
static Node* equality();
static Node* relational();
static Node* add();
static Node* mul();
static Node* primary();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_num(int num);
static Node* new_node_lvar(Ident* ident);

Function* function(){
    Function* func = calloc(1, sizeof(Function));
    
    scope_in();
    Node head = {};
    Node* cur = &head;
    while(!is_eof()){
        cur->next = stmt();
        cur = cur->next;
    }

    func->stmts = head.next;
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
    }

    Node* node = expr();
    expect_token(TK_SEMICORON);
    return node;
}

static Node* expr(){
    return assign();
}

static Node* assign(){
    Node* node = equality();
    
    if(consume_token(TK_ASSIGN)){
        node = new_node(ND_ASSIGN, node, assign());
    }
    return node;
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
    Node* node = add();
    while(true){
        if(consume_token(TK_L_ANGLE_BRACKET)){
            node = new_node(ND_LT, node, add());
        } else if(consume_token(TK_L_ANGLE_BRACKET_EQUAL)){
            node = new_node(ND_LE, node, add());
        } else if(consume_token(TK_R_ANGLE_BRACKET)){
            node = new_node(ND_LT, add(), node);
        } else if(consume_token(TK_R_ANGLE_BRACKET_EQUAL)){
            node = new_node(ND_LE, add(), node);
        } else {
            return node;
        }
    }
}

static Node* add(){
    Node* node = mul();

    while(true){
        if(consume_token(TK_ADD)){
            node = new_node(ND_ADD, node, mul());
        } else if(consume_token(TK_SUB)){
            node = new_node(ND_SUB, node, mul());
        } else {
            return node;
        }
    }
}

static Node* mul(){
    Node* node = primary();

    while(true){
        if(consume_token(TK_MUL)){
            node = new_node(ND_MUL, node, primary());
        } else if(consume_token(TK_DIV)){
            node = new_node(ND_DIV, node, primary());
        } else {
            break;
        }
    }

    return node;
}

static Node* primary(){
    if(consume_token(TK_L_PAREN)){
        Node* node = expr();
        expect_token(TK_R_PAREN);
        return node;
    }

    Token* ident_token = consume_ident();
    if(ident_token){
        Ident* ident = find_ident(ident_token);
        if(!ident){
            ident = declare_ident(ident_token, 8, ID_LVAR);
        }
        Node* node = new_node_lvar(ident);
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
