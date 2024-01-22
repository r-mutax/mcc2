#include "gen_ir.h"
#include "error.h"
#include <stdlib.h>

static long g_label = 0;
static IR head;
static IR* ir = &head;
static IR* new_IR(IRKind kind);
static void gen_function(Function* func);
static void gen_stmt(Node* stmt);
static void gen_expr(Node* node);
static void gen_lvar(Node* lvar);
static long get_label();

void gen_ir(Function* func){
    Function* cur = func;
    while(func){
        gen_function(func);
        func = func->next;
    }
}

static void gen_function(Function* func){
    new_IR(IR_FN_LABEL)->name = func->name;
    new_IR(IR_FN_START)->size = func->stack_size;

    Parameter* param = func->params;
    for(int i = 0;
        param; ++i, param = param->next){

        IR* ir = new_IR(IR_LVAR);
        ir->address = param->ident->offset;
        ir->size = param->ident->size;
        new_IR(IR_STORE_ARGREG)->val = i;
        new_IR(IR_ASSIGN);

        if(i >= 6){
            error_at(func->name->tok->pos, "Functions with more than six arguments are not supported.");
        }
    }

    Node* cur = func->stmts;
    while(cur){
        gen_stmt(cur);
        cur = cur->next;
    }
    new_IR(IR_FN_END);
}

static void gen_stmt(Node* node){
    switch(node->kind){
        case ND_RETURN:
            gen_expr(node->lhs);
            new_IR(IR_POP);
            new_IR(IR_FN_END);
            break;
        case ND_IF:
        {
            long l_end = get_label();
            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_end;
            gen_stmt(node->then);
            new_IR(IR_LABEL)->val = l_end;
            break;
        }
        case ND_IF_ELSE:
        {
            long l_else = get_label();
            long l_end = get_label();
            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_else;
            gen_stmt(node->then);
            new_IR(IR_JMP)->val = l_end;

            new_IR(IR_LABEL)->val = l_else;
            gen_stmt(node->elif);
            new_IR(IR_LABEL)->val = l_end;
            break;
        }
        case ND_WHILE:
        {
            long l_start = get_label();
            long l_end = get_label();

            new_IR(IR_LABEL)->val = l_start;
            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_end;
            gen_stmt(node->body);
            new_IR(IR_JMP)->val = l_start;
            new_IR(IR_LABEL)->val = l_end;
            break;
        }
        case ND_FOR:
        {
            long l_start = get_label();
            long l_end = get_label();

            gen_expr(node->init);
            new_IR(IR_LABEL)->val = l_start;
            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_end;
            gen_stmt(node->body);
            gen_expr(node->incr);
            new_IR(IR_JMP)->val = l_start;
            new_IR(IR_LABEL)->val = l_end;
            break;
        }
        case ND_BLOCK:
        {
            for(Node* cur = node->body; cur; cur = cur->next){
                gen_stmt(cur);
            }
            break;
        }
        default:
            gen_expr(node);
            // スタックトップに値が残っているはずなので、消しておく
            new_IR(IR_POP);
            break;
    }
}

// 変数のアドレスを計算してスタックに積む
static void gen_lvar(Node* node){
    switch(node->kind){
        case ND_LVAR:
            {
                IR* ir = new_IR(IR_LVAR);
                ir->address = node->ident->offset;
                ir->size = node->ident->size;
                return;
            }
        case ND_DREF:
            gen_expr(node->lhs);
            break;
        default:
            error_at(node->ident->tok->pos, "Not a lhs.\n");
            break;
    }
    return;
}

static void gen_expr(Node* node){
    switch(node->kind){
        case ND_NUM:
        {
            IR* ir = new_IR(IR_NUM);
            ir->val = node->val;
            return;
        }
        case ND_LVAR:
            gen_lvar(node);
            new_IR(IR_LOAD);
            return;
        case ND_ASSIGN:
            gen_lvar(node->lhs);
            gen_expr(node->rhs);
            new_IR(IR_ASSIGN);
            return;
        case ND_FUNCCALL:
            new_IR(IR_FN_CALL_NOARGS)->name = node->ident;
            return;
        case ND_ADDR:
            gen_lvar(node->lhs);
            return;
        case ND_DREF:
            gen_expr(node->lhs);
            new_IR(IR_DREF);
            return;
        case ND_LOGIC_OR:
        {
            long l_true = get_label();
            long l_end = get_label();

            gen_expr(node->lhs);
            new_IR(IR_JNZ)->val = l_true;
            gen_expr(node->rhs);
            new_IR(IR_JNZ)->val = l_true;

            // false側
            new_IR(IR_NUM)->val = 0;
            new_IR(IR_JMP)->val = l_end;
            
            new_IR(IR_LABEL)->val = l_true;
            new_IR(IR_NUM)->val = 1;
            new_IR(IR_LABEL)->val = l_end;
            return;
        }
        case ND_LOGIC_AND:
        {
            long l_false = get_label();
            long l_end = get_label();

            gen_expr(node->lhs);
            new_IR(IR_JZ)->val = l_false;
            gen_expr(node->rhs);
            new_IR(IR_JZ)->val = l_false;

            // true側
            new_IR(IR_NUM)->val = 1;
            new_IR(IR_JMP)->val = l_end;
            // false側
            new_IR(IR_LABEL)->val = l_false;
            new_IR(IR_NUM)->val = 0;
            new_IR(IR_LABEL)->val = l_end;
            return;
        }
        case ND_COND_EXPR:
        {
            long l_false = get_label();
            long l_end = get_label();

            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_false;
            gen_expr(node->lhs);
            new_IR(IR_JMP)->val = l_end;
            new_IR(IR_LABEL)->val = l_false;
            gen_expr(node->rhs);
            new_IR(IR_LABEL)->val = l_end;
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
        case ND_BIT_AND:
            new_IR(IR_BIT_AND);
            break;
        case ND_BIT_XOR:
            new_IR(IR_BIT_XOR);
            break;
        case ND_BIT_OR:
            new_IR(IR_BIT_OR);
            break;
        case ND_MOD:
            new_IR(IR_MOD);
            break;
        case ND_L_BITSHIFT:
            new_IR(IR_L_BIT_SHIFT);
            break;
        case ND_R_BITSHIFT:
            new_IR(IR_R_BIT_SHIFT);
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

static long get_label(){
    return g_label++;
}
