#include "mcc2.h"

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

int debug_regis = 1;

/*
    CAST CMD のルール
    1. 拡張長方向のキャスト
        <32 bit : 32bit
        64bit : 64bit
    2. 縮小方向のキャストはターゲットのサイズにする

    キャストコマンドの命名ルール
    (src_type)_(dst_type)
*/
typedef enum
{
    // to char
    NO_NEED = 0,
    i64_i8,
    i64_i16,
    i64_i32,
    i64_u8,
    i64_u16,
    i64_u32,

    u64_i8,
    u64_i16,
    u64_i32,
    u64_u8,
    u64_u16,
    u64_u32,
} CAST_CMD;

// cast instruction table
// cast_table[src][dst]のように使う
// レジスタは基本64bit幅に出している。
static CAST_CMD cast_table[][8] = {
// dst              i8       i16       i32       i64        u8       u16       u32       u64
/* s:i8     */ {    NO_NEED, NO_NEED,  NO_NEED,  NO_NEED,   i64_u8,  i64_u16,  i64_u32,  NO_NEED  },
/* s:i16    */ {    i64_i8,  NO_NEED,  NO_NEED,  NO_NEED,   i64_u8,  i64_u16,  i64_u32,  NO_NEED  },
/* s:i32    */ {    i64_i8,  i64_i16,  NO_NEED,  NO_NEED,   i64_u8,  i64_u16,  i64_u32,  NO_NEED  },
/* s:i64    */ {    i64_i8,  i64_i16,  i64_i32,  NO_NEED,   i64_u8,  i64_u16,  i64_u32,  NO_NEED  },

/* s:u8     */ {    i64_i8,  NO_NEED,  NO_NEED,  NO_NEED,   NO_NEED, NO_NEED,  NO_NEED,  NO_NEED  },
/* s:u16    */ {    i64_i8,  i64_i16,  NO_NEED,  NO_NEED,   u64_u8,  NO_NEED,  NO_NEED,  NO_NEED  },
/* s:u32    */ {    i64_i8,  i64_i16,  i64_i32,  NO_NEED,   u64_u8,  u64_u16,  NO_NEED,  NO_NEED  },
/* s:u64    */ {    i64_i8,  i64_i16,  i64_i32,  NO_NEED,   u64_u8,  u64_u16,  u64_u32,  NO_NEED  },
};

static void gen_cast_x86(Reg* t, Reg* s1, CAST_CMD);

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
                print("  mov %s, %lu\n", reg->rreg, reg->val);
            } else {
                reg->rreg = format_string("%lu\0", reg->val);
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
                    if(ident->is_static){
                        if(size == 1){
                            print("  movsx %s, BYTE PTR[.L%s]\n", reg->rreg, ident->name);
                        } else if(size == 2){
                            print("  movsx %s, WORD PTR[.L%s]\n", reg->rreg, ident->name);
                        } else if(size == 8){
                            print("  mov %s, QWORD PTR[.L%s]\n", reg->rreg, ident->name);
                        }
                    } else {
                        if(size == 1){
                            print("  movsx %s, BYTE PTR[%s]\n", reg->rreg, ident->name);
                        } else if(size == 2){
                            print("  movsx %s, WORD PTR[%s]\n", reg->rreg, ident->name);
                        } else if(size == 8){
                            print("  mov %s, QWORD PTR[%s]\n", reg->rreg, ident->name);
                        }
                    }
                }  else if(ident->kind == ID_GVAR && ident->is_string_literal){
                    print("  lea %s, [ rip + %s ]\n", reg->rreg, ident->name);
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

static SIZE_TYPE_ID get_size_type_id(Reg* reg)
{
    SIZE_TYPE_ID id = 0;
    switch(reg->size){
        case 1:
            id = reg->is_unsigned ? u8 : i8;
            break;
        case 2:
            id = reg->is_unsigned ? u16 : i16;
            break;
        case 4:
            id = reg->is_unsigned ? u32 : i32;
            break;
        case 8:
            id = reg->is_unsigned ? u64 : i64;
            break;
        default:
            id = ierr;
            break;
    }

    return id;
}

static void gen_cast_x86(Reg* t, Reg* s1, CAST_CMD cmd){
    print("# cast st\n");

    activateRegRhs(s1);
    activateRegLhs(t);

    switch(cmd){
        case NO_NEED:
            print("  mov %s, %s\n", t->rreg, s1->rreg);
            break;
        case i64_i8:
            print("  movsx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case i64_i16:
            print("  movsx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case i64_i32:
            print("  movsxd %s, %s\n", t->rreg, rreg32[s1->idx]);
            break;
        case u64_i8:
            print("  movzx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case u64_i16:
            print("  movzx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case u64_i32:
            print("  mov %s, %s\n", rreg32[t->idx], rreg32[s1->idx]);
            break;
        case i64_u8:
            print("  movzx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case i64_u16:
            print("  movzx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case i64_u32:
            print("  mov %s, %s\n", rreg32[t->idx], rreg32[s1->idx]);
            break;
        case u64_u8:
            print("  movzx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case u64_u16:
            print("  movzx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case u64_u32:
            print("  mov %s, %s\n", rreg32[t->idx], rreg32[s1->idx]);
            break;
        default:
            error("invalid cast %d \n", cmd);
    }
    freeReg(s1);
}

void gen_x86(IR* ir){
    print(".intel_syntax noprefix\n");

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
            case IR_VA_START:
            {
                int offset = ir->t->val;
                int gp = ir->s1->val;
                int fp = ir->s2->val;

                // +----------------------------+----------------------------+
                // | gp_offset (4 byte)         | fp_offset (4 byte)         | 0
                // +----------------------------+----------------------------+
                // | overflow_arg_area (8 byte)                              | 8
                // +---------------------------------------------------------+
                // | reg_save_area (8 byte)                                  | 16
                // +---------------------------------------------------------+
                // | register save area (8 * 6 byte)                         | 24
                // +---------------------------------------------------------+
                // | float register save area (8 * 8 byte)                   | 56
                // +---------------------------------------------------------+

                // va_elemの初期化
                print("  mov DWORD PTR [rbp - %d], %d\n",       offset, gp * 8);        // gp_offset
                print("  mov DWORD PTR [rbp - %d + 4], %d\n",   offset, 48 + fp * 8);   // fp_offset
                print("  lea rax, [rbp - %d + 16]\n",           offset);                // overflow_arg_area
                print("  mov QWORD PTR [rbp - %d + 8], rax\n",  offset);
                print("  lea rax, [rbp - %d + 24]\n",           offset);                // reg_save_area
                print("  mov QWORD PTR [rbp - %d + 16], rax\n", offset);
                
                // 引数の退避
                print("  mov QWORD PTR [rbp - %d + 24], rdi\n", offset);
                print("  mov QWORD PTR [rbp - %d + 32], rsi\n", offset);
                print("  mov QWORD PTR [rbp - %d + 40], rdx\n", offset);
                print("  mov QWORD PTR [rbp - %d + 48], rcx\n", offset);
                print("  mov QWORD PTR [rbp - %d + 56], r8\n",  offset);
                print("  mov QWORD PTR [rbp - %d + 64], r9\n",  offset);

                print("  movsd QWORD PTR [rbp - %d + 72], xmm0\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 80], xmm1\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 88], xmm2\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 96], xmm3\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 104], xmm4\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 112], xmm5\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 120], xmm6\n", offset);
                print("  movsd QWORD PTR [rbp - %d + 128], xmm7\n", offset);
            }
                break;
            case IR_EXTERN_LABEL:
                print(".global %s\n", ir->s1->str);
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
                } else if(ident->is_static) {
                    print("  .bss\n");
                    print(".L%s:\n", ir->s1->ident->name);
                    print("  .zero %d\n", ir->s2->val);
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
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);

                // cmp命令は直値は32bit幅までしか受け取れないので、
                // 左辺値として割り当てる
                activateRegLhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  sete al\n");
                print("  movzb %s, al\n", ir->t->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_NOT_EQUAL:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("  cmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("  setne al\n");
                print("  movzb %s, al\n", ir->t->rreg);
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
                

                if(ir->t){
                    activateRegLhs(ir->t);
                    print("  mov %s, %s\n", ir->t->rreg, ir->s2->rreg);
                }
                freeReg(ir->s2);
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
                    if(ir->s1->ident->is_static){
                        print("  lea %s, [ rip + .L%s ]\n", ir->t->rreg, ir->s1->ident->name);
                    } else {
                        print("  lea %s, [ rip + %s ]\n", ir->t->rreg, ir->s1->ident->name);
                    }
                }
                break;
            case IR_CAST:
                {
                    SIZE_TYPE_ID dst_id = get_size_type_id(ir->t);
                    SIZE_TYPE_ID src_id = get_size_type_id(ir->s1);

                    if(dst_id == ierr || src_id == ierr){
                        error("invalid cast\n");
                    }

                    CAST_CMD cmd = cast_table[src_id][dst_id];
                    gen_cast_x86(ir->t, ir->s1, cmd);
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
                if(ir->s2->is_unsigned){
                    if(ir->s2->size == 1){
                        print("  movzx %s, BYTE PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 2){
                        print("  movzx %s, WORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 4){
                        print("  mov %s, DWORD PTR [%s]\n", rreg32[ir->s1->idx], ir->s2->rreg);
                    } else if(ir->s2->size == 8){
                        print("  mov %s, QWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    }
                } else {
                    if(ir->s2->size == 1){
                        print("  movsx %s, BYTE PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 2){
                        print("  movsx %s, WORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 4){
                        print("  movsxd %s, DWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 8){
                        print("  mov %s, QWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    }
                }
                freeReg(ir->s2);
                break;
            case IR_COMMENT:
                print("#");
                printline(ir->s1->tok);
                if(debug_regis)
                {
                    print("#");
                    int remreg = 6;
                    for(int i = 0; i < 6; i++){
                        if(realReg[i]){
                            remreg--;
                        }
                    }
                    print(" remain register num : %d\n", remreg);
                }

                break;
            default:
                unreachable();
        }
        ir = ir->next;
    }
}
