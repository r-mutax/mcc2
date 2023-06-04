#include "mcc2.h"
#include "tokenizer.h"
#include <stdbool.h>
#include <stdlib.h>

static Node* mul();
static Node* primary();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_num(int num);

Node* expr(){
    Node* node = mul();

    while(true){
        if(consume_token(TK_ADD)){
            node = new_node(ND_ADD, node, mul());
        } else if(consume_token(TK_SUB)){
            node = new_node(ND_SUB, node, mul());
        } else {
            break;
        }
    }

    return node;
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
