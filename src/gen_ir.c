#include "gen_ir.h"
#include <stdlib.h>

IR* ir;
static IR* new_IR(IRKind kind, IR* cur);

void gen_ir(Node* node){
    IR head = {};
    IR* cur = &head;

    switch(node->kind){
        case ND_NUM:
            cur = new_IR(IR_NUM, cur);
            return;
        default:
            break;
    }

    gen_ir(node->lhs);
    gen_ir(node->rhs);

    switch(node->kind){
        case ND_ADD:
            cur = new_IR(IR_ADD, cur);
            break;
        case ND_SUB:
            cur = new_IR(IR_SUB, cur);
            break;
        default:
            break;
    }

    ir = head.next;
}

static IR* new_IR(IRKind kind, IR* cur){
    IR* ir = calloc(1, sizeof(IR));
    ir->kind = kind;
    cur->next = ir;
    return ir;
}

