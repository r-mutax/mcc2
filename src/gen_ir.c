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
static void gen_lvar(Node* lvar){
    if(lvar->kind != ND_LVAR){
        error_at(lvar->ident->tok->pos, "Not a lhs.\n");
    }
    IR* ir = new_IR(IR_LVAR);
    ir->address = lvar->ident->offset;
    ir->size = lvar->ident->size;
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
