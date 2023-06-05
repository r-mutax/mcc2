#include "gen_x86_64.h"
#include "gen_ir.h"
#include <stdio.h>

static void pop2();

void gen_x86(IR* ir){
    FILE* fp = stdout;

    fprintf(fp, ".intel_syntax noprefix\n");
    fprintf(fp, ".global main\n");
    fprintf(fp, "main:\n");
    while(ir){
        switch(ir->kind){
            case IR_NUM:
                fprintf(fp, "  mov rax, %d\n", ir->val);
                fprintf(fp, "  push rax\n");
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
            case IR_POP:
                fprintf(fp, "  pop rax\n");
                break;
        }

        ir = ir->next;
    }


    // ひとまず、スタックトップの値をリターンすることにする
    fprintf(fp, "  ret\n");
}

static void pop2(FILE* fp){
    fprintf(fp, "  pop rdi\n");
    fprintf(fp, "  pop rax\n");
}