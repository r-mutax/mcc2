#include "gen_ir.h"
#include <stdlib.h>

IR head;
IR* ir = &head;
static IR* new_IR(IRKind kind);
static void gen_stmt(Node* stmt);
static void gen_expr(Node* node);

void gen_ir(Function* func){
    Node* cur = func->stmts;
    while(cur){
        gen_stmt(cur);
        cur = cur->next;
    }
}

static void gen_stmt(Node* stmt){
    gen_expr(stmt);

    // スタックトップに値が残っているはずなので、消しておく
    new_IR(IR_POP);
}

static void gen_expr(Node* node){
    switch(node->kind){
        case ND_NUM:
        {
            IR* ir = new_IR(IR_NUM);
            ir->val = node->val;
            return;
        }
        default:
            break;
    }

    gen_expr(node->lhs);
    gen_expr(node->rhs);

    switch(node->kind){
        case ND_ADD:
            new_IR(IR_ADD);
            break;
        case ND_SUB:
            new_IR(IR_SUB);
            break;
        case ND_MUL:
            new_IR(IR_MUL);
            break;
        case ND_DIV:
            new_IR(IR_DIV);
            break;
        case ND_EQUAL:
            new_IR(IR_EQUAL);
            break;
        case ND_NOT_EQUAL:
            new_IR(IR_NOT_EQUAL);
            break;
        case ND_LT:
            new_IR(IR_LT);
            break;
        case ND_LE:
            new_IR(IR_LE);
            break;
        default:
            break;
    }

    return;
}

IR* get_ir(){
    return head.next;
}

static IR* new_IR(IRKind kind){
    IR* tmp = calloc(1, sizeof(IR));
    tmp->kind = kind;
    ir->next = tmp;
    ir = tmp;
    return tmp;
}

