#include "mcc2.h"
#include "tokenizer.h"
#include <stdbool.h>

Node* expr();
static Node* primary();
static Node* new_node(NodeKind kind, Node* lhs, Node* rhs);
static Node* new_node_num(int num);

Node* expr(){
    Node* node = primary();

    while(true){
        if(consume_token(TK_ADD)){
            node = new_node(ND_ADD, node, primary());
        } else if(consume_token(TK_SUB)){
            node = new_node(ND_SUB, node, primary());
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
