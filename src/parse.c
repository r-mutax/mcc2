#include "mcc2.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdlib.h>

/*
    expr = equality
    equality = add ('==' add | '!=' add)*
    add = mul ('+' mul | '-' mul)*
    mul = unary ('*' unary | '/' unary)
    unary = ('+' | '-')? primary
    primary = '(' expr ')' | num
*/

static Node* equality();
static Node* add();
static Node* mul();
static Node* primary();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_num(int num);

Node* expr(){
    return equality();
}

static Node* equality(){
    Node* node = add();

    while(true){
        if(consume_token(TK_EQUAL)){
            node = new_node(ND_EQUAL, node, add());
        } else if(consume_token(TK_NOT_EQUAL)){
            node = new_node(ND_NOT_EQUAL, node, add());
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
