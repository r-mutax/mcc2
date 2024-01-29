#include "gen_x86_64.h"
#include "gen_ir.h"
#include "tokenizer.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>

static const char *argreg8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
static const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

static void pop2();

void gen_x86(IR* ir){
    FILE* fp = stdout;

    fprintf(fp, ".intel_syntax noprefix\n");
    fprintf(fp, ".global main\n");
    while(ir){
        switch(ir->kind){
            case IR_FN_LABEL:
            {
                fprintf(fp, "  .text\n");
                char* str = get_token_string(ir->name->tok);
                fprintf(fp, "%s:\n", str);
                free(str);
            }
                break;
            case IR_FN_START:
                fprintf(fp, "  push rbp\n");
                fprintf(fp, "  mov rbp, rsp\n");
                fprintf(fp, "  sub rsp, %d\n", ((ir->size + 15) / 16) * 16);
                break;
            case IR_FN_END:
                fprintf(fp, "  mov rsp, rbp\n");
                fprintf(fp, "  pop rbp\n");
                fprintf(fp, "  ret\n");
                break;
            case IR_FN_CALL_NOARGS:
                {
                    char* str = get_token_string(ir->name->tok);
                    fprintf(fp, "  call %s\n", str);
                    fprintf(fp, "  push rax\n");
                    free(str);
                    break;
                }
            case IR_NUM:
                fprintf(fp, "  mov rax, %d\n", ir->val);
                fprintf(fp, "  push rax\n");
                break;
            case IR_LVAR:
                fprintf(fp, "  lea rax, [rbp - %d]\n", ir->address);
                // fprintf(fp, "  mov rax, [rax]\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_GVAR:
                fprintf(fp, "  mov rax, OFFSET FLAT:%s\n", get_token_string(ir->name->tok));
                fprintf(fp, "  push rax\n");
                break;
            case IR_LOAD:
                fprintf(fp, "  pop rax\n");
                if(ir->size == 1){
                    fprintf(fp, "  movsx eax, BYTE PTR [rax]\n");
                } else if(ir->size == 8){
                    fprintf(fp, "  mov rax, [rax]\n");
                }
                fprintf(fp, "  push rax\n");
                break;
            case IR_ASSIGN:
                fprintf(fp, "  pop rdi\n");
                fprintf(fp, "  pop rax\n");

                if(ir->size == 1){
                    fprintf(fp, "  mov [rax], dil\n");
                } else if(ir->size == 8){
                    fprintf(fp, "  mov [rax], rdi\n");
                }
                fprintf(fp, "  push rdi\n");
                break;
            case IR_ADD:
                pop2(fp);
                fprintf(fp, "  add rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_SUB:
                pop2(fp);
                fprintf(fp, "  sub rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_MUL:
                pop2(fp);
                fprintf(fp, "  imul rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_DIV:
                pop2(fp);
                fprintf(fp, "  cqo\n");
                fprintf(fp, "  idiv rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_MOD:
                pop2(fp);
                fprintf(fp, "  cqo\n");
                fprintf(fp, "  idiv rdi\n");
                fprintf(fp, "  mov rax, rdx\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_EQUAL:
                pop2(fp);
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  sete al\n");
                fprintf(fp, "  movzb rax, al\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_NOT_EQUAL:
                pop2(fp);
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  setne al\n");
                fprintf(fp, "  movzb rax, al\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_LT:
                pop2(fp);
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  setl al\n");
                fprintf(fp, "  movzb rax, al\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_LE:
                pop2(fp);
                fprintf(fp, "  cmp rax, rdi\n");
                fprintf(fp, "  setle al\n");
                fprintf(fp, "  movzb rax, al\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_BIT_AND:
                pop2(fp);
                fprintf(fp, "  and rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_BIT_XOR:
                pop2(fp);
                fprintf(fp, "  xor rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_BIT_OR:
                pop2(fp);
                fprintf(fp, "  or rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_L_BIT_SHIFT:
                pop2(fp);
                fprintf(fp, "  mov rcx, rdi\n");
                fprintf(fp, "  sal rax, cl\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_R_BIT_SHIFT:
                pop2(fp);
                fprintf(fp, "  mov rcx, rdi\n");
                fprintf(fp, "  sar rax, cl\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_POP:
                fprintf(fp, "  pop rax\n");
                break;
            case IR_LABEL:
                fprintf(fp, ".L%d:\n", ir->val);
                break;
            case IR_JMP:
                fprintf(fp, "  jmp .L%d\n", ir->val);
            case IR_JZ:
                fprintf(fp, "  pop rax\n");
                fprintf(fp, "  cmp rax, 0\n");
                fprintf(fp, "  je .L%d\n", ir->val);
                break;
            case IR_JNZ:
                fprintf(fp, "  pop rax\n");
                fprintf(fp, "  cmp rax, 0\n");
                fprintf(fp, "  jne .L%d\n", ir->val);
                break;
            case IR_STORE_ARGREG:
                fprintf(fp, "  pop rax\n");
                if(ir->size == 1){
                    fprintf(fp, "  mov [rax], %s\n", argreg8[ir->val]);
                } else if(ir->size == 8){
                    fprintf(fp, "  mov [rax], %s\n", argreg64[ir->val]);
                }
                break;
            case IR_LOAD_ARGREG:
                fprintf(fp, "  pop %s\n", argreg64[ir->val]);
                break;
            case IR_DREF:
                fprintf(fp, "  pop rax\n");
                fprintf(fp, "  mov rax, [rax]\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_GVAR_DEF:
                fprintf(fp, "  .bss\n");
                fprintf(fp, "%s:\n", get_token_string(ir->name->tok));
                fprintf(fp, "  .zero %d\n", ir->name->type->size);
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

static void pop2(FILE* fp){
    fprintf(fp, "  pop rdi\n");
    fprintf(fp, "  pop rax\n");
}