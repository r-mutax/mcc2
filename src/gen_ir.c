#include "gen_ir.h"
#include "ident.h"
#include "tokenizer.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>

static long g_label = 0;
static long g_break = -1;
static long g_continue = -1;
static void gen_datas(Ident* ident);
static void gen_funcs(Ident* ident);
static void gen_function(Ident* func);
static void gen_stmt(Node* stmt);
static Reg* gen_lvar(Node* lvar);
static long get_label();

// レジスタマシン
static IR head;
static IR* ir = &head;
static IR* new_IR(IRCmd cmd, Reg* t, Reg* s1, Reg* s2);
static IR* new_IRLabel(long label);
static IR* new_IRJmp(long label);
static Reg* new_Reg();
static Reg* new_RegImm(int val);
static Reg* new_RegVar(Ident* ident);
static Reg* new_RegStr(char* str);
static Reg* new_RegAddr(Reg* reg, int size);
static Reg* new_RegFname(Ident* ident);
static Reg* gen_expr(Node* node);

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
            new_IR(IR_GVAR_LABEL, NULL, new_RegVar(ident), new_RegImm(ident->type->size));
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
    new_IR(IR_FN_LABEL, NULL, new_RegStr(func->name), new_RegImm(func->stack_size));

    // パラメータの展開
    Parameter* param = func->params;
    for(int i = 0;
        param; ++i, param = param->next){

        new_IR(IR_STORE_ARG_REG, NULL, new_RegVar(param->ident), new_RegImm(i));
        if(i >= 6){
            error_at(func->tok->pos, "Functions with more than six arguments are not supported.");
        }
    }

    Scope* scope = func->scope;
    for(Label* label = scope->label; label; label = label->next){
        if(label->labeld){
            label->no = get_label();
        }
    }

    Node* cur = func->funcbody;
    while(cur){
        gen_stmt(cur);
        cur = cur->next;
    }
}

static void gen_stmt(Node* node){
    if(node->pos){
        new_IR(IR_COMMENT, NULL, new_RegStr(node->pos->pos), NULL);
    }
    switch(node->kind){
        case ND_RETURN:
            new_IR(IR_RET, NULL, gen_expr(node->lhs), NULL);
            break;
        case ND_IF:
        {
            long l_end = get_label();
            new_IR(IR_JZ, NULL, gen_expr(node->cond), new_RegImm(l_end));
            gen_stmt(node->then);
            new_IRLabel(l_end);
            break;
        }
        case ND_IF_ELSE:
        {
            long l_else = get_label();
            long l_end = get_label();

            new_IR(IR_JZ, NULL, gen_expr(node->cond), new_RegImm(l_else));
            gen_stmt(node->then);
            new_IRJmp(l_end);

            new_IRLabel(l_else);
            gen_stmt(node->elif);
            new_IRLabel(l_end);
            break;
        }
        case ND_WHILE:
        {
            long l_break_buf = g_break;
            long l_continue_buf = g_continue;
            long l_start = get_label();
            long l_end = get_label();

            new_IRLabel(l_start);
            new_IR(IR_JZ, NULL, gen_expr(node->cond), new_RegImm(l_end));

            g_continue = l_start;
            g_break = l_end;
            gen_stmt(node->body);
            g_break = l_break_buf;
            g_continue = l_continue_buf;

            new_IRJmp(l_start);
            new_IRLabel(l_end);
            break;
        }
        case ND_DO_WHILE:
        {
            long l_break_buf = g_break;
            long l_continue_buf = g_continue;
            long l_start = get_label();
            long l_end = get_label();

            new_IRLabel(l_start);
            g_continue = l_start;
            g_break = l_end;
            gen_stmt(node->body);
            g_break = l_break_buf;
            g_continue = l_continue_buf;

            new_IR(IR_JNZ, NULL, gen_expr(node->cond), new_RegImm(l_start));
            new_IRLabel(l_end);
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
            new_IRLabel(l_start);
            new_IR(IR_JZ, NULL, gen_expr(node->cond), new_RegImm(l_end));

            g_continue = l_cont;
            g_break = l_end;
            gen_stmt(node->body);
            g_break = l_break_buf;
            g_continue = l_continue_buf;

            new_IRLabel(l_cont);
            if(node->incr){
                gen_expr(node->incr);
            }
            new_IRJmp(l_start);
            new_IRLabel(l_end);
            break;
        }
        case ND_SWITCH:{
            long l_break_buf = g_break;
            long l_end = get_label();

            // caseのどこに飛ぶか？の分岐の部分
            Reg* reg = gen_expr(node->cond);
            for( Node* nc = node->next_case; nc; nc = nc->next_case){
                nc->val = get_label();
                new_IR(IR_JE, new_RegImm(nc->val), reg, gen_expr(nc->lhs));
            }

            // default
            if(node->default_label){
                node->default_label->val = get_label();
                new_IR(IR_JMP, NULL, new_RegImm(node->default_label->val), NULL);
            }

            // body
            g_break = l_end;
            gen_stmt(node->body);
            g_break = l_break_buf;

            new_IRLabel(l_end);
            break;
        }
        case ND_CASE:{
            new_IRLabel(node->val);
            break;
        }
        case ND_BREAK:
            if(g_break != -1){
                new_IRJmp(g_break);
            }
            break;
        case ND_DEFAULT:
            new_IRLabel(node->val);
            break;
        case ND_CONTINUE:
            if(g_continue != -1){
                new_IRJmp(g_continue);
            }
            break;
        case ND_LABEL:
            new_IRLabel(node->label->no);
            break;
        case ND_GOTO:
            if(node->label->labeld){
                new_IRJmp(node->label->no);
            } else {
                error_at(node->pos->pos, "Label don't defined.\n");
            }
            break;
        case ND_BLOCK:
        {
            for(Node* cur = node->body; cur; cur = cur->next){
                gen_stmt(cur);
            }
            break;
        }
        case ND_VOID_STMT:
            // 空文なのでなにもしない
            break;
        default:
            gen_expr(node);
            break;
    }
    new_IR(IR_RELEASE_REG, NULL, NULL, NULL);
}

static Reg* gen_lvar(Node*  node){
    switch(node->kind){
        case ND_VAR:
        {
            Reg* reg = new_Reg();
            new_IR(IR_REL, reg, new_RegVar(node->ident), NULL);
            reg->size = node->type->size;
            return reg;
        }
            return new_RegVar(node->ident);
        case ND_DREF:
            return gen_expr(node->lhs);
        case ND_MEMBER:
        {
            Reg* base_reg = gen_lvar(node->lhs);
            base_reg->size = node->type->size;
            new_IR(IR_ADD, NULL, base_reg, new_RegImm(node->val));
            return base_reg;
        }
        default:
            error_at(node->ident->tok->pos, "Not a lhs.\n");
            break;
    }
    return NULL;
}

static Reg* gen_expr(Node* node){
    switch(node->kind){
        case ND_NUM: {
            return new_RegImm(node->val);
        }
        case ND_VAR:
        case ND_MEMBER:
        {
            Reg* regvar = gen_lvar(node);
            if(node->type->kind != TY_ARRAY){
                Reg* reg = new_Reg();
                new_IR(IR_LOAD, NULL, reg, regvar);
                return reg;
            }
            return regvar;
        }
            return new_RegVar(node->ident);
        case ND_FUNCCALL:
        {
            int nargs = 0;
            for(Node* cur = node->params; cur; cur = cur->next){
                Reg* reg = gen_expr(cur);
                new_IR(IR_LOAD_ARG_REG, new_RegImm(nargs++), reg, NULL);
            }
            if(node->ident->is_var_params){
                new_IR(IR_SET_FLOAT_NUM, NULL, new_RegImm(0), NULL);
            }
            Reg* ret = new_Reg();
            new_IR(IR_FN_CALL, ret, new_RegFname(node->ident), NULL);
            return ret;
        }
        case ND_ADDR:
        {
            return gen_lvar(node->lhs);
        }
        case ND_DREF:
        {
            Reg* ret = new_Reg();
            new_IR(IR_LOAD, NULL, ret, gen_expr(node->lhs));
            return ret;
        }
        case ND_COMMA:
        {
            gen_expr(node->lhs);
            return gen_expr(node->rhs);
        }
        case ND_ASSIGN:
        {
            if(node->lhs->type->kind == TY_ARRAY){
                error("incompatible types in assignment to array.");
            }
            Reg* reg = new_Reg();
            new_IR(IR_ASSIGN, reg, gen_lvar(node->lhs), gen_expr(node->rhs));
            return reg;
        }
        case ND_LOGIC_OR:
        {
            Reg* ret = new_Reg();
            long l_true = get_label();
            long l_end = get_label();
            new_IR(IR_JNZ, NULL, gen_expr(node->lhs), new_RegImm(l_true));
            new_IR(IR_JNZ, NULL, gen_expr(node->rhs), new_RegImm(l_true));
            
            new_IR(IR_MOV, NULL, ret, new_RegImm(0));
            new_IR(IR_JMP, NULL, new_RegImm(l_end), NULL);
            
            new_IRLabel(l_true);
            new_IR(IR_MOV, NULL, ret, new_RegImm(1));            
            new_IRLabel(l_end);
            return ret;
        }
        case ND_LOGIC_AND:
        {
            Reg* ret = new_Reg();
            long l_false = get_label();
            long l_end = get_label();
            new_IR(IR_JZ, NULL, gen_expr(node->lhs), new_RegImm(l_false));
            new_IR(IR_JZ, NULL, gen_expr(node->rhs), new_RegImm(l_false));

            new_IR(IR_MOV, NULL, ret, new_RegImm(1));
            new_IR(IR_JMP, NULL, new_RegImm(l_end), NULL);

            new_IRLabel(l_false);
            new_IR(IR_MOV, NULL, ret, new_RegImm(0));
            new_IRLabel(l_end);
            return ret;
        }
        case ND_COND_EXPR:
        {
            Reg* ret = new_Reg();
            long l_false = get_label();
            long l_end = get_label();

            new_IR(IR_JZ, NULL, gen_expr(node->cond), new_RegImm(l_false));

            new_IR(IR_MOV, NULL, ret, gen_expr(node->lhs));
            new_IR(IR_JMP, NULL, new_RegImm(l_end), NULL);

            new_IRLabel(l_false);
            new_IR(IR_MOV, NULL, ret, gen_expr(node->rhs));            
            new_IRLabel(l_end);
            return ret;
        }
        default:
            break;
    }
    
    Reg* r1 = gen_expr(node->lhs);
    Reg* r2 = gen_expr(node->rhs);

    Reg* ret = r1;
    switch(node->kind){
        case ND_ADD:
            new_IR(IR_ADD, NULL, r1, r2);
            break;
        case ND_SUB:
            new_IR(IR_SUB, NULL, r1, r2);
            break;
        case ND_MUL:
            new_IR(IR_MUL, NULL, r1, r2);
            break;
        case ND_DIV:
            new_IR(IR_DIV, NULL, r1, r2);
            break;
        case ND_MOD:
            new_IR(IR_MOD, NULL, r1, r2);
            break;
        case ND_EQUAL:
            new_IR(IR_EQUAL, NULL, r1, r2);
            break;
        case ND_NOT_EQUAL:
            new_IR(IR_NOT_EQUAL, NULL, r1, r2);
            break;
        case ND_LT:
            {
                ret = new_Reg();
                new_IR(IR_LT, ret, r1, r2);            
            }
            break;
        case ND_LE:
            {
                ret = new_Reg();
                new_IR(IR_LE, ret, r1, r2);            
            }
            break;
        case ND_BIT_AND:
            new_IR(IR_BIT_AND, NULL, r1, r2);
            break;
        case ND_BIT_XOR:
            new_IR(IR_BIT_XOR, NULL, r1, r2);
            break;
        case ND_BIT_OR:
            new_IR(IR_BIT_OR, NULL, r1, r2);
            break;
        case ND_L_BITSHIFT:
            new_IR(IR_L_BIT_SHIFT, NULL, r1, r2);
            break;
        case ND_R_BITSHIFT:
            new_IR(IR_R_BIT_SHIFT, NULL, r1, r2);
            break;
        default:
            break;
    }
    
    return ret;
}

IR* get_ir(){
    return head.next;
}

static long get_label(){
    return g_label++;
}

static IR* new_IR(IRCmd cmd, Reg* t, Reg* s1, Reg* s2){
    IR* r = calloc(1, sizeof(IR));
    r->cmd = cmd;
    r->t = t;
    r->s1 = s1;
    r->s2 = s2;
    ir->next = r;
    ir = r;
    return r;
}

static IR* new_IRLabel(long label){
    new_IR(IR_LABEL, NULL, new_RegImm(label), NULL);
}

static IR* new_IRJmp(long label){
    new_IR(IR_JMP, NULL, new_RegImm(label), NULL);
}

static Reg* new_Reg(){
    Reg* reg = calloc(1, sizeof(Reg));
    reg->idx = -1;
    reg->size = 8;
    return reg;
}

static Reg* new_RegImm(int val){
    Reg* reg = new_Reg();
    reg->kind = REG_IMM;
    reg->val = val;
    return reg;
}

static Reg* new_RegVar(Ident* ident){
    Reg* reg = new_Reg();
    reg->kind = REG_VAR;
    reg->ident = ident;
    reg->size = ident->type->size;
    return reg;
}

// for label
static Reg* new_RegStr(char* str){
    Reg* reg = new_Reg();
    reg->kind = REG_STR;
    reg->str = str;
    return reg;
}

static Reg* new_RegAddr(Reg* addr, int size){
    Reg* reg = new_Reg();
    reg->kind = REG_ADDR;
    reg->addr = addr;
    reg->size = size;
    return reg;
}

static Reg* new_RegFname(Ident* ident){
    Reg* reg = new_Reg();
    reg->kind = REG_FNAME;
    reg->ident = ident;
    return reg;
}