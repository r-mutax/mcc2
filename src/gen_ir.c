#include "gen_ir.h"
#include "ident.h"
#include "error.h"
#include <stdlib.h>

static long g_label = 0;
static long g_break = -1;
static long g_continue = -1;
static IR head;
static IR* ir = &head;
static IR* new_IR(IRKind kind);
static void gen_datas(Ident* ident);
static void gen_funcs(Ident* ident);
static void gen_function(Ident* func);
static void gen_stmt(Node* stmt);
static void gen_expr(Node* node);
static void gen_lvar(Node* lvar);
static long get_label();

void gen_ir(){
    // グローバル変数の出力
    Scope* scope = get_global_scope();
    Ident* ident = scope->ident;

    gen_datas(ident);
    gen_funcs(ident);
}

static void gen_datas(Ident* ident){
    while(ident){
        if(ident->kind == ID_GVAR){
            IR* ir = new_IR(IR_GVAR_DEF);
            ir->name = ident;
        }
        ident = ident->next;
    }
}

static void gen_funcs(Ident* ident){
    while(ident){
        if(ident->kind == ID_FUNC && ident->funcbody){
            gen_function(ident);
        }
        ident = ident->next;
    }
}


static void gen_function(Ident* func){
    new_IR(IR_FN_LABEL)->name = func;
    new_IR(IR_FN_START)->size = func->stack_size;

    Parameter* param = func->params;
    for(int i = 0;
        param; ++i, param = param->next){

        IR* ir = new_IR(IR_LVAR);
        ir->address = param->ident->offset;
        IR* ir2 = new_IR(IR_STORE_ARGREG);
        ir2->val = i;
        ir2->size = param->ident->type->size;
        if(i >= 6){
            error_at(func->tok->pos, "Functions with more than six arguments are not supported.");
        }
    }

    Node* cur = func->funcbody;
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
            long l_break_buf = g_break;
            long l_continue_buf = g_continue;
            long l_start = get_label();
            long l_end = get_label();

            new_IR(IR_LABEL)->val = l_start;
            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_end;

            g_continue = l_start;
            g_break = l_end;
            gen_stmt(node->body);
            g_break = l_break_buf;
            g_continue = l_continue_buf;

            new_IR(IR_JMP)->val = l_start;
            new_IR(IR_LABEL)->val = l_end;
            break;
        }
        case ND_FOR:
        {
            long l_break_buf = g_break;
            long l_continue_buf = g_continue;
            long l_start = get_label();
            long l_cont = get_label();
            long l_end = get_label();

            if(node->init){
                gen_expr(node->init);
            }
            new_IR(IR_LABEL)->val = l_start;
            gen_expr(node->cond);
            new_IR(IR_JZ)->val = l_end;

            g_continue = l_cont;
            g_break = l_end;
            gen_stmt(node->body);
            g_break = l_break_buf;
            g_continue = l_continue_buf;

            new_IR(IR_LABEL)->val = l_cont;
            if(node->incr){
                gen_expr(node->incr);
            }
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
        case ND_BREAK:
            if(g_break != -1){
                new_IR(IR_JMP)->val = g_break;
            }
            break;
        case ND_CONTINUE:
            if(g_continue != -1){
                new_IR(IR_JMP)->val = g_continue;
            }
            break;
        case ND_VOID_STMT:
            // 空文なのでなにもしない
            break;
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
        case ND_VAR:
            if(node->ident->kind == ID_LVAR){
                IR* ir = new_IR(IR_LVAR);
                ir->address = node->ident->offset;
                ir->size = node->ident->type->size;
                return;
            }
            else if(node->ident->kind == ID_GVAR){
                IR* ir = new_IR(IR_GVAR);
                ir->name = node->ident;
                ir->size = node->ident->type->size;
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
        case ND_VAR:
            gen_lvar(node);
            if(node->type->kind != TY_ARRAY){
                new_IR(IR_LOAD)->size = node->ident->type->size;
            }
            return;
        case ND_ASSIGN:
            if(node->lhs->type->kind == TY_ARRAY){
                error("incompatible types in assignment to array.");
            }

            gen_lvar(node->lhs);
            gen_expr(node->rhs);
            new_IR(IR_ASSIGN)->size = node->lhs->type->size;
            return;
        case ND_FUNCCALL:
            {
                int nargs = 0;
                for(Node* cur = node->params; cur; cur = cur->next){
                    gen_expr(cur);
                    nargs++;
                }

                for(;nargs; nargs--){
                    IR* ir = new_IR(IR_LOAD_ARGREG);
                    ir->size = 8;
                    ir->val = nargs - 1;
                }

                if(node->ident->is_var_params){
                    new_IR(IR_SET_FLOAT_NUM)->val = 0;
                }
                new_IR(IR_FN_CALL_NOARGS)->name = node->ident;
                return;
            }
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
