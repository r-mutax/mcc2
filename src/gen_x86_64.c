#include "gen_x86_64.h"
#include "gen_ir.h"
#include <stdio.h>

void gen_x86(IR* ir){
    FILE* fp = stdout;

    fprintf(fp, ".intel_syntax noprefix\n");
    fprintf(fp, ".global main\n");
    fprintf(fp, "main:\n");
    while(ir){
        switch(ir->kind){
            case IR_NUM:
                fprintf(fp, "  push %d\n", ir->val);
                break;
            case IR_ADD:
                fprintf(fp, "  pop rdi\n");
                fprintf(fp, "  pop rax\n");
                fprintf(fp, "  add rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
            case IR_SUB:
                fprintf(fp, "  pop rdi\n");
                fprintf(fp, "  pop rax\n");
                fprintf(fp, "  sub rax, rdi\n");
                fprintf(fp, "  push rax\n");
                break;
        }

        ir = ir->next;
    }


    // ひとまず、スタくトップの値をリターンすることにする
    fprintf(fp, "  pop rax\n");
    fprintf(fp, "  ret\n");
}