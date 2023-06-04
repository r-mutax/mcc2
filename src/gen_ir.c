#include "gen_ir.h"
#include <stdlib.h>

IR head;
IR* ir = &head;
static IR* new_IR(IRKind kind);

void gen_ir(Node* node){
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

    gen_ir(node->lhs);
    gen_ir(node->rhs);

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

