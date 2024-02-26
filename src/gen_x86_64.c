#include "gen_x86_64.h"
#include "gen_ir.h"
#include "tokenizer.h"
#include "error.h"
#include "utility.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *rreg8[] = {"r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
static const char *rreg16[] = {"r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
static const char *rreg64[] = {"r10", "r11", "r12", "r13", "r14", "r15"};
static Reg* realReg[6] = { } ;

static const char *argreg8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
static const char *argreg16[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
static const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
int depth = 0;
FILE* fp;

static void pop(char* reg);
static void push(char* reg);

static int findReg();

static void activateRegLhs(Reg* reg);
static void activateRegRhs(Reg* reg);
static void activateReg(Reg* reg, int is_lhs);

static void freeReg(Reg* reg);

static void pop(char* reg){
    fprintf(fp, "  pop %s\n", reg);
    --depth;
}

static void push(char* reg){
    fprintf(fp, "  push %s\n", reg);
    --depth;
}

static int findReg(){
    for(int i = 0; i < 6; i++){
        if(!realReg[i]){
            return i;
        }
    }
    error("full of register\n");
    return -1;
}

// レジスタを割り当てる
static void assignReg(Reg* reg){
    if(reg->rreg) return;

    int idx = findReg();
    reg->idx = idx;
    realReg[idx] = reg;
    reg->rreg = format_string("%s", rreg64[idx]);
}

// レジスタを左辺値としてアクティベートする
static void activateRegLhs(Reg* reg){
    activateReg(reg, 1);
}

// レジスタを右辺値としてアクティベートする
static void activateRegRhs(Reg* reg){
    activateReg(reg, 0);
}

// レジスタをアクティベートする
static void activateReg(Reg* reg, int is_lhs){
    if(reg->rreg) return;

    switch(reg->kind){
        case REG_IMM:
            if(is_lhs){
                assignReg(reg);
                fprintf(fp, "  mov %s, %d\n", reg->rreg, reg->val);
            } else {
                reg->rreg = format_string("%d\0", reg->val);
            }
            break;
        case REG_REG:
            assignReg(reg);
            break;
        case REG_VAR:
            {
                assignReg(reg);
                Ident* ident = reg->ident;
                int size = ident->type->size;

                if(ident->kind == ID_LVAR){
                    if(size == 1){
                        fprintf(fp, "  movsx %s, BYTE PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    } else if(size == 2){
                        fprintf(fp, "  movsx %s, WORD PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    } else if(size == 8){
                        fprintf(fp, "  mov %s, QWORD PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    }
                } else if(ident->kind == ID_GVAR){
                    if(size == 1){
                        fprintf(fp, "  movsx %s, BYTE PTR[%s]\n", reg->rreg, ident->name);
                    } else if(size == 2){
                        fprintf(fp, "  movsx %s, WORD PTR[%s]\n", reg->rreg, ident->name);
                    } else if(size == 8){
                        fprintf(fp, "  mov %s, QWORD PTR[%s]\n", reg->rreg, ident->name);
                    }
                }  else if(ident->kind == ID_GVAR && ident->is_string_literal){
                    fprintf(fp, "  mov %s, OFFSET FLAT:%s\n", reg->rreg, ident->name);
                }
            }            
            break;
    }
}


static void freeReg(Reg* reg){
    free(reg->rreg);
    reg->rreg = NULL;

    if(reg->idx == -1) return;

    realReg[reg->idx] = NULL;
    reg->idx = -1;
}

static void freeRegAll(Reg* t, Reg* s1, Reg* s2){
    if(t){
        freeReg(s1);
    }
    freeReg(s2);
}

static void freeRegAllForce(){
    for(int i = 0; i < 6; i++){
        if(realReg[i]){
            freeReg(realReg[i]);
        }
    }
}

static void emit_binop(char* op, Reg* t, Reg* s1, Reg* s2){
    activateRegLhs(s1);
    activateRegRhs(s2);
    fprintf(fp, "  %s %s, %s\n", op, s1->rreg, s2->rreg);

    if(t){
        // あるならそっちにmovが入る
        freeReg(s1);
    } else {
        // s1は残しておく
    }
    freeReg(s2);
}

void gen_x86(IR* ir){
    fp = stdout;
    fprintf(fp, ".intel_syntax noprefix\n");
    fprintf(fp, ".global main\n");

    while(ir){
        switch(ir->cmd){
            case IR_FN_LABEL:
                fprintf(fp, "  .text\n");
                fprintf(fp, "%s:\n", ir->s1->str);
                push("rbp");
                fprintf(fp, "  mov rbp, rsp\n");
                fprintf(fp, "  sub rsp, %d\n", ((ir->s2->val + 15) / 16) * 16);
                push("r12");
                push("r13");
                push("r14");
                push("r15");
                break;
            case IR_STORE_ARG_REG:

                if(ir->s1->ident->type->size == 1){
                    fprintf(fp, "  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg8[ir->s2->val]);
                } else if(ir->s1->ident->type->size == 2){
                    fprintf(fp, "  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg16[ir->s2->val]);
                } else if(ir->s1->ident->type->size == 8){
                    fprintf(fp, "  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg64[ir->s2->val]);
                }
                break;
            case IR_LOAD_ARG_REG:
                activateRegLhs(ir->s1);
                fprintf(fp, "  mov %s, %s\n", argreg64[ir->t->val], ir->s1->rreg);
                freeReg(ir->s1);
                break;
            case IR_SET_FLOAT_NUM:
                fprintf(fp, "  mov eax, %d\n", '\0');
                break;
            case IR_RET:
                if(ir->s1){
                    activateRegLhs(ir->s1);
                    fprintf(fp, "  mov rax, %s\n", ir->s1->rreg);
                }
                pop("r15");
                pop("r14");
                pop("r13");
                pop("r12");
                fprintf(fp, "  mov rsp, rbp\n");
                pop("rbp");
                fprintf(fp, "  ret\n");
                break;
            case IR_GVAR_LABEL:
            {
                Ident* ident = ir->s1->ident;
                if(ident->is_string_literal){
                    fprintf(fp, "  .data\n");
                    fprintf(fp, "%s:\n", ident->name);
                    fprintf(fp, "  .string \"%s\"\n", get_token_string(ident->tok));
                } else {
                    fprintf(fp, "  .bss\n");
                    fprintf(fp, "%s:\n", ir->s1->ident->name);
                    fprintf(fp, "  .zero %d\n", ir->s2->val);
                }
                break;
            }
            case IR_ADD:
                emit_binop("add", ir->t, ir->s1, ir->s2);
                break;
            case IR_SUB:
                emit_binop("sub", ir->t, ir->s1, ir->s2);
                break;
            case IR_MUL:
                emit_binop("imul", ir->t, ir->s1, ir->s2);
                break;
            case IR_DIV:
                activateRegLhs(ir->s1);

                // idivが受け取るoperandはレジスタなので、
                // 左辺値として割り当てる
                activateRegLhs(ir->s2);
                fprintf(fp, "  mov rax, %s\n", ir->s1->rreg);
                fprintf(fp, "  cqo\n");
                fprintf(fp, "  idiv %s\n", ir->s2->rreg);
                fprintf(fp, "  mov %s, rax\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_MOD:
                activateRegLhs(ir->s1);

                // idivが受け取るoperandはレジスタなので、
                // 左辺値として割り当てる
                activateRegLhs(ir->s2);
                fprintf(fp, "  mov rax, %s\n", ir->s1->rreg);
                fprintf(fp, "  cqo\n");
                fprintf(fp, "  idiv %s\n", ir->s2->rreg);
                fprintf(fp, "  mov %s, rdx\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_EQUAL:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                fprintf(fp, "  sete al\n");
                fprintf(fp, "  movzb %s, al\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_NOT_EQUAL:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                fprintf(fp, "  setne al\n");
                fprintf(fp, "  movzb %s, al\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_LT:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                fprintf(fp, "  setl al\n");
                fprintf(fp, "  movzb %s, al\n", ir->t->rreg);
                freeReg(ir->s1);
                freeReg(ir->s2);
                break;
            case IR_LE:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                fprintf(fp, "  setle al\n");
                fprintf(fp, "  movzb %s, al\n", ir->t->rreg);
                freeReg(ir->s1);
                freeReg(ir->s2);
                break;
            case IR_BIT_AND:
                emit_binop("and", ir->t, ir->s1, ir->s2);
                break;
            case IR_BIT_XOR:
                emit_binop("xor", ir->t, ir->s1, ir->s2);
                break;
            case IR_BIT_OR:
                emit_binop("or", ir->t, ir->s1, ir->s2);
                break;
            case IR_L_BIT_SHIFT:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->kind == REG_IMM){
                    fprintf(fp, "  sal %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                } else {
                    fprintf(fp, "  mov rcx, %s\n", ir->s2->rreg);
                    fprintf(fp, "  sal %s, cl\n", ir->s1->rreg);
                }
                if(ir->s2->kind == REG_IMM){
                    freeRegAll(ir->t, ir->s1, ir->s2);
                }
                break;
            case IR_R_BIT_SHIFT:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->kind == REG_IMM){
                    fprintf(fp, "  sar %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                } else {
                    fprintf(fp, "  mov rcx, %s\n", ir->s2->rreg);
                    fprintf(fp, "  sar %s, cl\n", ir->s1->rreg);
                }
                if(ir->s2->kind == REG_IMM){
                    freeRegAll(ir->t, ir->s1, ir->s2);
                }
                break;
            case IR_ASSIGN:
                activateRegLhs(ir->s1);
                activateRegLhs(ir->s2);
                if(ir->s1->size == 1){
                    fprintf(fp, "  mov [%s], %s\n", ir->s1->rreg, rreg8[ir->s2->idx]);
                } else if(ir->s1->size == 2){
                    fprintf(fp, "  mov [%s], %s\n", ir->s1->rreg, rreg16[ir->s2->idx]);
                } else if(ir->s1->size == 8){
                    fprintf(fp, "  mov [%s], %s\n", ir->s1->rreg, rreg64[ir->s2->idx]);
                }
                
                freeReg(ir->s2);

                if(ir->t){
                    activateRegLhs(ir->t);
                    fprintf(fp, "  mov %s, [%s]\n", ir->t->rreg, ir->s1->rreg);
                }
                freeReg(ir->s1);
                break;
            case IR_FN_CALL:
                {
                    if(depth % 2){
                        fprintf(fp, "  sub rsp, 8\n");
                    }
                    fprintf(fp, "  call %s\n", ir->s1->ident->name);
                    if(depth % 2){
                        fprintf(fp, "  add rsp, 8\n");
                    }
                    activateRegLhs(ir->t);
                    fprintf(fp, "  mov %s, rax\n", ir->t->rreg);
                }
                break;
            case IR_REL:
                activateRegLhs(ir->t);
                if(ir->s1->ident->kind == ID_LVAR){
                    fprintf(fp, "  lea %s, [rbp - %d]\n", ir->t->rreg, ir->s1->ident->offset);
                } else if(ir->s1->ident->kind == ID_GVAR){
                    fprintf(fp, "  mov %s, OFFSET FLAT:%s\n", ir->t->rreg, ir->s1->ident->name);
                }
                break;
            case IR_MOV:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  mov %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_RELEASE_REG:
                fprintf(fp, "# free reg\n");
                freeRegAllForce();
                break;
            case IR_LABEL:
                fprintf(fp, ".L%d:\n", ir->s1->val);
                break;
            case IR_JNZ:
                activateRegLhs(ir->s1);
                //activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, 0\n", ir->s1->rreg);
                fprintf(fp, "  jne .L%d\n", ir->s2->val);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_JZ:
                activateRegLhs(ir->s1);
                //activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, 0\n", ir->s1->rreg);
                fprintf(fp, "  je .L%d\n", ir->s2->val);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_JE:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                fprintf(fp, "  je .L%d\n", ir->t->val);
                freeReg(ir->s2);
                break;
            case IR_JMP:
                fprintf(fp, "  jmp .L%d\n", ir->s1->val);
                break;
            case IR_LEA:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                fprintf(fp, "  lea %s, [rbp - %s]\n", ir->s1->rreg, ir->s2->rreg);
                freeReg(ir->s2);
                break;
            case IR_LOAD:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->size == 1){
                    fprintf(fp, "  mov %s, 0\n", ir->s1->rreg);
                    fprintf(fp, "  mov %s, [%s]\n", rreg8[ir->s1->idx], ir->s2->rreg);
                } else if(ir->s2->size == 2){
                    fprintf(fp, "  mov %s, 0\n", ir->s1->rreg);
                    fprintf(fp, "  mov %s, [%s]\n", rreg16[ir->s1->idx], ir->s2->rreg);
                } else if(ir->s2->size == 8){
                    fprintf(fp, "  mov %s, [%s]\n", ir->s1->rreg, ir->s2->rreg);
                }
                freeReg(ir->s2);
                break;
            case IR_COMMENT:
                fprintf(fp, "#");
                printline(ir->s1->str);
                break;
            default:
                unreachable();
        }
        ir = ir->next;
    }
    
}
