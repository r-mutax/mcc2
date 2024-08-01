#include "gen_x86_64.h"
#include "gen_ir.h"
#include "tokenizer.h"
#include "error.h"
#include "utility.h"
#include "file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *rreg8[] = {"r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
static const char *rreg16[] = {"r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
static const char *rreg32[] = {"r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
static const char *rreg64[] = {"r10", "r11", "r12", "r13", "r14", "r15"};
static Reg* realReg[6] = { } ;

static const char *argreg8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
static const char *argreg16[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
static const char *argreg32[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
static const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
int depth = 0;

static void pop(char* reg);
static void push(char* reg);

static int findReg();

static void activateRegLhs(Reg* reg);
static void activateRegRhs(Reg* reg);
static void activateReg(Reg* reg, int is_lhs);

static void freeReg(Reg* reg);

static void pop(char* reg){
    print("  pop %s\n", reg);
    --depth;
}

static void push(char* reg){
    print("  push %s\n", reg);
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
                print("  mov %s, %d\n", reg->rreg, reg->val);
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
                        print("  movsx %s, BYTE PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    } else if(size == 2){
                        print("  movsx %s, WORD PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    } else if(size == 8){
                        print("  mov %s, QWORD PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    }
                } else if(ident->kind == ID_GVAR){
                    if(size == 1){
                        print("  movsx %s, BYTE PTR[%s]\n", reg->rreg, ident->name);
                    } else if(size == 2){
                        print("  movsx %s, WORD PTR[%s]\n", reg->rreg, ident->name);
                    } else if(size == 8){
                        print("  mov %s, QWORD PTR[%s]\n", reg->rreg, ident->name);
                    }
                }  else if(ident->kind == ID_GVAR && ident->is_string_literal){
                    print("  mov %s, OFFSET FLAT:%s\n", reg->rreg, ident->name);
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
    print("  %s %s, %s\n", op, s1->rreg, s2->rreg);

    if(t){
        // あるならそっちにmovが入る
        freeReg(s1);
    } else {
        // s1は残しておく
    }
    freeReg(s2);
}

void gen_x86(IR* ir){
    print(".intel_syntax noprefix\n");
    print(".global main\n");

    while(ir){
        switch(ir->cmd){
            case IR_FN_LABEL:
                print("  .text\n");
                print("%s:\n", ir->s1->str);
                push("rbp");
                print("  mov rbp, rsp\n");
                print("  sub rsp, %d\n", ((ir->s2->val + 15) / 16) * 16);
                push("r12");
                push("r13");
                push("r14");
                push("r15");
                break;
            case IR_FN_END_LABEL:
                print("ret_%s:\n", ir->s1->str);
                pop("r15");
                pop("r14");
                pop("r13");
                pop("r12");
                print("  mov rsp, rbp\n");
                pop("rbp");
                print("  ret\n");
                break;
            case IR_STORE_ARG_REG:

                if(ir->s1->ident->type->size == 1){
                    print("  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg8[ir->s2->val]);
                } else if(ir->s1->ident->type->size == 2){
                    print("  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg16[ir->s2->val]);
                } else if(ir->s1->ident->type->size == 4){
                    print("  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg32[ir->s2->val]);
                } else if(ir->s1->ident->type->size == 8){
                    print("  mov [rbp - %d], %s\n", ir->s1->ident->offset, argreg64[ir->s2->val]);
                }
                break;
            case IR_LOAD_ARG_REG:
                activateRegLhs(ir->s1);
                print("  mov %s, %s\n", argreg64[ir->t->val], ir->s1->rreg);
                freeReg(ir->s1);
                break;
            case IR_SET_FLOAT_NUM:
                print("  mov eax, %d\n", '\0');
                break;
            case IR_RET:
                if(ir->s1){
                    activateRegLhs(ir->s1);
                    print("  mov rax, %s\n", ir->s1->rreg);
                }
                print("  jmp ret_%s\n", ir->s2->str);
                break;
            case IR_GVAR_LABEL:
            {
                Ident* ident = ir->s1->ident;
                if(ident->is_string_literal){
                    print("  .data\n");
                    print("%s:\n", ident->name);
                    print("  .string \"%s\"\n", get_token_string(ident->tok));
                } else {
                    print("  .bss\n");
                    print("%s:\n", ir->s1->ident->name);
                    print("  .zero %d\n", ir->s2->val);
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
                print("  mov rax, %s\n", ir->s1->rreg);
                print("  cqo\n");
                print("  idiv %s\n", ir->s2->rreg);
                print("  mov %s, rax\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_MOD:
                activateRegLhs(ir->s1);

                // idivが受け取るoperandはレジスタなので、
                // 左辺値として割り当てる
                activateRegLhs(ir->s2);
                print("  mov rax, %s\n", ir->s1->rreg);
                print("  cqo\n");
                print("  idiv %s\n", ir->s2->rreg);
                print("  mov %s, rdx\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_EQUAL:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  sete al\n");
                print("  movzb %s, al\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_NOT_EQUAL:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  setne al\n");
                print("  movzb %s, al\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_LT:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  setl al\n");
                print("  movzb %s, al\n", ir->t->rreg);
                freeReg(ir->s1);
                freeReg(ir->s2);
                break;
            case IR_LE:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  setle al\n");
                print("  movzb %s, al\n", ir->t->rreg);
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
                    print("  sal %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                } else {
                    print("  mov rcx, %s\n", ir->s2->rreg);
                    print("  sal %s, cl\n", ir->s1->rreg);
                }
                if(ir->s2->kind == REG_IMM){
                    freeRegAll(ir->t, ir->s1, ir->s2);
                }
                break;
            case IR_R_BIT_SHIFT:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->kind == REG_IMM){
                    print("  sar %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                } else {
                    print("  mov rcx, %s\n", ir->s2->rreg);
                    print("  sar %s, cl\n", ir->s1->rreg);
                }
                if(ir->s2->kind == REG_IMM){
                    freeRegAll(ir->t, ir->s1, ir->s2);
                }
                break;
            case IR_ASSIGN:
                activateRegLhs(ir->s1);
                activateRegLhs(ir->s2);
                if(ir->s1->size == 1){
                    print("  mov [%s], %s\n", ir->s1->rreg, rreg8[ir->s2->idx]);
                } else if(ir->s1->size == 2){
                    print("  mov [%s], %s\n", ir->s1->rreg, rreg16[ir->s2->idx]);
                } else if(ir->s1->size == 4){
                    print("  mov [%s], %s\n", ir->s1->rreg, rreg32[ir->s2->idx]);
                } else if(ir->s1->size == 8){
                    print("  mov [%s], %s\n", ir->s1->rreg, rreg64[ir->s2->idx]);
                }
                freeReg(ir->s2);

                if(ir->t){
                    activateRegLhs(ir->t);
                    if(ir->s1->size == 1){
                        print("  movsx %s, BYTE PTR [%s]\n", ir->t->rreg, ir->s1->rreg);
                    } else if(ir->s1->size == 2){
                        print("  movsx %s, WORD PTR [%s]\n", ir->t->rreg, ir->s1->rreg);
                    } else if(ir->s1->size == 4){
                        print("  movsxd %s, DWORD PTR [%s]\n", ir->t->rreg, ir->s1->rreg);
                    } else if(ir->s1->size == 8){
                        print("  mov %s, QWORD PTR [%s]\n", ir->t->rreg, ir->s1->rreg);
                    }
                }
                freeReg(ir->s1);
                break;
            case IR_FN_CALL:
                {
                    if(depth % 2){
                        print("  sub rsp, 8\n");
                    }
                    print("  call %s\n", ir->s1->ident->name);
                    if(depth % 2){
                        print("  add rsp, 8\n");
                    }
                    activateRegLhs(ir->t);
                    print("  mov %s, rax\n", ir->t->rreg);
                }
                break;
            case IR_REL:
                activateRegLhs(ir->t);
                if(ir->s1->ident->kind == ID_LVAR){
                    print("  lea %s, [rbp - %d]\n", ir->t->rreg, ir->s1->ident->offset);
                } else if(ir->s1->ident->kind == ID_GVAR){
                    print("  mov %s, OFFSET FLAT:%s\n", ir->t->rreg, ir->s1->ident->name);
                }
                break;
            case IR_MOV:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  mov %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_RELEASE_REG:
                print("# free reg\n");
                freeRegAllForce();
                break;
            case IR_LABEL:
                print(".L%d:\n", ir->s1->val);
                break;
            case IR_JNZ:
                activateRegLhs(ir->s1);
                //activateRegRhs(ir->s2);
                print("  cmp %s, 0\n", ir->s1->rreg);
                print("  jne .L%d\n", ir->s2->val);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_JZ:
                activateRegLhs(ir->s1);
                //activateRegRhs(ir->s2);
                print("  cmp %s, 0\n", ir->s1->rreg);
                print("  je .L%d\n", ir->s2->val);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_JE:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  je .L%d\n", ir->t->val);
                freeReg(ir->s2);
                break;
            case IR_JMP:
                print("  jmp .L%d\n", ir->s1->val);
                break;
            case IR_LEA:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  lea %s, [rbp - %s]\n", ir->s1->rreg, ir->s2->rreg);
                freeReg(ir->s2);
                break;
            case IR_LOAD:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->size == 1){
                    print("  movsx %s, BYTE PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                } else if(ir->s2->size == 2){
                    print("  movsx %s, WORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                 } else if(ir->s2->size == 4){
                    print("  movsxd %s, DWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                } else if(ir->s2->size == 8){
                    print("  mov %s, QWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                }
                freeReg(ir->s2);
                break;
            case IR_COMMENT:
                print("#");
                printline(ir->s1->tok);
                break;
            default:
                unreachable();
        }
        ir = ir->next;
    }
}
