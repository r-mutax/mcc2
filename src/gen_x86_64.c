#include "gen_x86_64.h"
#include "gen_ir.h"
#include "tokenizer.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

static const char *argreg8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
static const char *argreg16[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
static const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
int depth = 0;
FILE* fp;

static void pop(char* reg);
static void push(char* reg);

static void pop(char* reg){
    fprintf(fp, "  pop %s\n", reg);
    --depth;
}

static void push(char* reg){
    fprintf(fp, "  push %s\n", reg);
    --depth;
}

void gen_x86(IR* ir){
    fp = stdout;

    fprintf(fp, ".intel_syntax noprefix\n");
    fprintf(fp, ".global main\n");
    while(ir){
        switch(ir->kind){
            case IR_FN_LABEL:
            {
                fprintf(fp, "  .text\n");
                //char* str = get_token_string(ir->name->name);
                fprintf(fp, "%s:\n", ir->name->name);
            }
                break;
            case IR_FN_START:
                push("rbp");
                fprintf(fp, "  mov rbp, rsp\n");
                fprintf(fp, "  sub rsp, %d\n", ((ir->size + 15) / 16) * 16);
                break;
            case IR_FN_END:
                fprintf(fp, "  mov rsp, rbp\n");
                pop("rbp");
                fprintf(fp, "  ret\n");
                break;
            case IR_FN_CALL_NOARGS:
                {
                    //char* str = get_token_string(ir->name->name);
                    if(depth % 2){
                        fprintf(fp, "  sub rsp, 8\n");
                    }
                    fprintf(fp, "  call %s\n", ir->name->name);
                    if(depth % 2){
                        fprintf(fp, "  add rsp, 8\n");
                    }
                    push("rax");
                    break;
                }
            case IR_NUM:
                fprintf(fp, "  mov rax, %d\n", ir->val);
                push("rax");
                break;
            case IR_LVAR:
                fprintf(fp, "  lea rax, [rbp - %d]\n", ir->address);
                push("rax");
                break;
            case IR_GVAR:
                fprintf(fp, "  mov rax, OFFSET FLAT:%s\n", ir->name->name);
                push("rax");
                break;
            case IR_LOAD:
                pop("rax");
                if(ir->size == 1){
                    fprintf(fp, "  movsx rax, BYTE PTR [rax]\n");
                } else if(ir->size == 2){
                    fprintf(fp, "  movsx rax, WORD PTR [rax]\n");
                } else if(ir->size == 8){
                    fprintf(fp, "  mov rax, [rax]\n");
                }
                push("rax");
                break;
            case IR_ASSIGN:
                pop("rdi");
                pop("rax");

                if(ir->size == 1){
                    fprintf(fp, "  mov [rax], dil\n");
                } else if(ir->size == 2){
                    fprintf(fp, "  mov [rax], di\n");
                } else if(ir->size == 8){
                    fprintf(fp, "  mov [rax], rdi\n");
                }
                push("rdi");
                break;
            case IR_ADD:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  add rax, rdi\n");
                push("rax");
                break;
            case IR_SUB:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  sub rax, rdi\n");
                push("rax");
                break;
            case IR_MUL:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  imul rax, rdi\n");
                push("rax");
                break;
            case IR_DIV:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  cqo\n");
                fprintf(fp, "  idiv rdi\n");
                push("rax");
                break;
            case IR_MOD:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  cqo\n");
                fprintf(fp, "  idiv rdi\n");
                fprintf(fp, "  mov rax, rdx\n");
                push("rax");
                break;
            case IR_EQUAL:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  sete al\n");
                fprintf(fp, "  movzb rax, al\n");
                push("rax");
                break;
            case IR_NOT_EQUAL:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  setne al\n");
                fprintf(fp, "  movzb rax, al\n");
                push("rax");
                break;
            case IR_LT:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  setl al\n");
                fprintf(fp, "  movzb rax, al\n");
                push("rax");
                break;
            case IR_LE:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  setle al\n");
                fprintf(fp, "  movzb rax, al\n");
                push("rax");
                break;
            case IR_BIT_AND:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  and rax, rdi\n");
                push("rax");
                break;
            case IR_BIT_XOR:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  xor rax, rdi\n");
                push("rax");
                break;
            case IR_BIT_OR:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  or rax, rdi\n");
                push("rax");
                break;
            case IR_L_BIT_SHIFT:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  mov rcx, rdi\n");
                fprintf(fp, "  sal rax, cl\n");
                push("rax");
                break;
            case IR_R_BIT_SHIFT:
                pop("rdi");
                pop("rax");
                fprintf(fp, "  mov rcx, rdi\n");
                fprintf(fp, "  sar rax, cl\n");
                push("rax");
                break;
            case IR_POP:
                pop("rax");
                break;
            case IR_LABEL:
                fprintf(fp, ".L%d:\n", ir->val);
                break;
            case IR_JMP:
                fprintf(fp, "  jmp .L%d\n", ir->val);
            case IR_JZ:
                pop("rax");
                fprintf(fp, "  cmp rax, 0\n");
                fprintf(fp, "  je .L%d\n", ir->val);
                break;
            case IR_JNZ:
                pop("rax");
                fprintf(fp, "  cmp rax, 0\n");
                fprintf(fp, "  jne .L%d\n", ir->val);
                break;
            case IR_STORE_ARGREG:
                pop("rax");
                if(ir->size == 1){
                    fprintf(fp, "  mov [rax], %s\n", argreg8[ir->val]);
                } else if(ir->size == 2){
                    fprintf(fp, "  mov [rax], %s\n", argreg16[ir->val]);
                } else if(ir->size == 8){
                    fprintf(fp, "  mov [rax], %s\n", argreg64[ir->val]);
                }
                break;
            case IR_LOAD_ARGREG:
                pop((char*)argreg64[ir->val]);
                break;
            case IR_SET_FLOAT_NUM:
                fprintf(fp, "  mov eax, %d\n", ir->val);
                break;
            case IR_DREF:
                pop("rax");
                fprintf(fp, "  mov rax, [rax]\n");
                push("rax");
                break;
            case IR_GVAR_DEF:
                if(ir->name->is_string_literal){
                    fprintf(fp, "  .data\n");
                    fprintf(fp, "%s:\n", ir->name->name);
                    fprintf(fp, "  .string \"%s\"\n", get_token_string(ir->name->tok));
                } else {
                    fprintf(fp, "  .bss\n");
                    fprintf(fp, "%s:\n", ir->name->name);
                    fprintf(fp, "  .zero %d\n", ir->name->type->size);
                }
                break;
            default:
                unreachable();
                break;
        }

        ir = ir->next;
    }


    // ひとまず、スタックトップの値をリターンすることにする
    //fprintf(fp, "  ret\n");
}
