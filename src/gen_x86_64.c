#include "mcc2.h"

static void convert_ir2x86asm(IR* ir);                  // IR->x86アセンブリ変換
static void dprint_Ident(Ident* ident, int level);     // 識別子のデバッグ出力

static const char *rreg8[] = {"r10b", "r11b", "r12b", "r13b", "r14b", "r15b"};
static const char *rreg16[] = {"r10w", "r11w", "r12w", "r13w", "r14w", "r15w"};
static const char *rreg32[] = {"r10d", "r11d", "r12d", "r13d", "r14d", "r15d"};
static const char *rreg64[] = {"r10", "r11", "r12", "r13", "r14", "r15"};
static Reg* realReg[6] = { } ;

static int spillReg[30] = { } ; // 仮想レジスタの退避領域（1: 退避済み, 0: 未退避）
static int useReg[3] = { -1 };     // 処理対象の中間命令で使用する実レジスタのインデックス

static const char *argreg8[] = {"dil", "sil", "dl", "cl", "r8b", "r9b"};
static const char *argreg16[] = {"di", "si", "dx", "cx", "r8w", "r9w"};
static const char *argreg32[] = {"edi", "esi", "edx", "ecx", "r8d", "r9d"};
static const char *argreg64[] = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
int depth = 1;
static int file_label = 1;
static int func_id = 1;

int debug_regis = 0;    // レジスタのデバッグモード
int debug_plvar = 0;    // ローカル変数のデバッグモード
int debug_exec = 0;     // 実行時のデバッグモード

typedef enum {
    NONE = 0,
    TEXT,
    DATA,
    BSS,
} SECTION;
static SECTION g_section = NONE;
static void set_section(SECTION set){
    if(g_section == set){
        return;
    }
    switch(set){
        case TEXT:
            print("\t.text\n");
            break;
        case DATA:
            print("\t.data\n");
            break;
        case BSS:
            print("\t.bss\n");
            break;
        default:
            break;
    }
    g_section = set;
}

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

// DRAWF規格に従ったレジスタのidxを取得する
static int get_regno(char* reg){
    if(!strcmp(reg, "rax")) return 0;
    if(!strcmp(reg, "rdx")) return 1;
    if(!strcmp(reg, "rcx")) return 2;
    if(!strcmp(reg, "rbx")) return 3;
    if(!strcmp(reg, "rsi")) return 4;
    if(!strcmp(reg, "rdi")) return 5;
    if(!strcmp(reg, "rbp")) return 6;
    if(!strcmp(reg, "rsp")) return 7;
    if(!strcmp(reg, "r8")) return 8;
    if(!strcmp(reg, "r9")) return 9;
    if(!strcmp(reg, "r10")) return 10;
    if(!strcmp(reg, "r11")) return 11;
    if(!strcmp(reg, "r12")) return 12;
    if(!strcmp(reg, "r13")) return 13;
    if(!strcmp(reg, "r14")) return 14;
    if(!strcmp(reg, "r15")) return 15;
    return -1;
}

static void pop(char* reg){
    print("\tpop %s\n", reg);
    --depth;

    int idx = get_regno(reg);
}

static void push(char* reg){
    print("\tpush %s\n", reg);
    ++depth;
    int idx = get_regno(reg);
}

static int findSpillReg(){
    for(int i = 0; i < 30; i++){
        if(!spillReg[i]){
            return i;
        }
    }
    error("full of spill register\n");
    return -1;
}

static int findReg(){
    for(int i = 0; i < 6; i++){
        if(!realReg[i]){
            return i;
        }
    }

    // 実レジスタをすべて使用しているので、spillする
    // 1. spill先を見つける。
    int spill_idx = findSpillReg();

    // 2. spillするレジスタを見つける。realRegを見て、markしていないものを探す
    int src_reg_idx = -1;
    for(int i = 0; i < 6; i++){
        if(realReg[i]){
            bool is_use = false;
            for(int j = 0; j < 3; j++){
                if(useReg[j] == i){
                    is_use = true;
                    break;
                }
            }
            if(is_use) continue;
            // 割当済の実レジスタで、使用中でないレジスタのidxを取得
            src_reg_idx = i;
            break;
        }
    }

    // 3. spillする
    if(debug_regis){
        print("# spill %s to [rbp - %d]\n", rreg64[src_reg_idx], 8 * spill_idx);
    }

    print("\tmov QWORD PTR [rbp - 240 + %d], %s\n", 8 * spill_idx, rreg64[src_reg_idx]);
    spillReg[spill_idx] = 1;
    realReg[src_reg_idx]->spill_idx = spill_idx;

    return src_reg_idx;
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
    if(reg->spill_idx != -1){
        // スピルレジスタからロードする
        if(debug_regis){
            print("# back from spill [rbp - %d] to %s\n", 8 * reg->spill_idx, rreg64[reg->idx]);
        }

        print("\tmov %s, QWORD PTR [rbp - 240 + %d]\n", rreg64[reg->idx], 8 * reg->spill_idx);
        spillReg[reg->spill_idx] = 0;
        reg->spill_idx = -1;
        return;
    }

    if(reg->rreg) return;

    switch(reg->kind){
        case REG_IMM:
            if(is_lhs){
                assignReg(reg);
                print("\tmov %s, %lu\n", reg->rreg, reg->val);
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
                int size = get_qtype_size(ident->qtype);

                if(ident->kind == ID_LVAR){
                    if(size == 1){
                        print("\tmovsx %s, BYTE PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    } else if(size == 2){
                        print("\tmovsx %s, WORD PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    } else if(size == 8){
                        print("\tmov %s, QWORD PTR[rbp - %d]\n", reg->rreg, ident->offset);
                    }
                } else if(ident->kind == ID_GVAR){
                    if(ident->is_static){
                        if(size == 1){
                            print("\tmovsx %s, BYTE PTR[.L%s]\n", reg->rreg, ident->name);
                        } else if(size == 2){
                            print("\tmovsx %s, WORD PTR[.L%s]\n", reg->rreg, ident->name);
                        } else if(size == 8){
                            print("\tmov %s, QWORD PTR[.L%s]\n", reg->rreg, ident->name);
                        }
                    } else {
                        if(size == 1){
                            print("\tmovsx %s, BYTE PTR[%s]\n", reg->rreg, ident->name);
                        } else if(size == 2){
                            print("\tmovsx %s, WORD PTR[%s]\n", reg->rreg, ident->name);
                        } else if(size == 8){
                            print("\tmov %s, QWORD PTR[%s]\n", reg->rreg, ident->name);
                        }
                    }
                }  else if(ident->kind == ID_GVAR && ident->is_string_literal){
                    print("\tlea %s, [ rip + %s ]\n", reg->rreg, ident->name);
                }
            }
            break;
    }

    // 使用するレジスタはマークする
    for(int i = 0; i < 3; i++){
        if(useReg[i] == -1){
            useReg[i] = reg->idx;
            break;
        }
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
    print("\t%s %s, %s\n", op, s1->rreg, s2->rreg);

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
            print("\tmov %s, %s\n", t->rreg, s1->rreg);
            break;
        case i64_i8:
            print("\tmovsx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case i64_i16:
            print("\tmovsx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case i64_i32:
            print("\tmovsxd %s, %s\n", t->rreg, rreg32[s1->idx]);
            break;
        case u64_i8:
            print("\tmovzx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case u64_i16:
            print("\tmovzx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case u64_i32:
            print("\tmov %s, %s\n", rreg32[t->idx], rreg32[s1->idx]);
            break;
        case i64_u8:
            print("\tmovzx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case i64_u16:
            print("\tmovzx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case i64_u32:
            print("\tmov %s, %s\n", rreg32[t->idx], rreg32[s1->idx]);
            break;
        case u64_u8:
            print("\tmovzx %s, %s\n", t->rreg, rreg8[s1->idx]);
            break;
        case u64_u16:
            print("\tmovzx %s, %s\n", t->rreg, rreg16[s1->idx]);
            break;
        case u64_u32:
            print("\tmov %s, %s\n", rreg32[t->idx], rreg32[s1->idx]);
            break;
        default:
            error("invalid cast %d \n", cmd);
    }
    freeReg(s1);
}

void gen_x86(){
    Scope* global_scope = get_global_scope();

    print("\t.intel_syntax noprefix\n");
    print("\t.file \"%s\"\n", cinfo.compile_file);
    set_section(TEXT);
    print(".Ltext0:\n");

    // extern宣言
    IR* ir = global_scope->ir_cmd;
    // convert_ir2x86asm(ir);

    // グローバル変数の出力
    Ident* ident = global_scope->ident;
    while(ident){
        if(ident->kind == ID_GVAR && !ident->is_extern){
            convert_ir2x86asm(ident->ir_cmd);
        }
        ident = ident->next;
    }

    // 関数定義
    ident = global_scope->ident;
    while(ident){
        if(ident->kind == ID_FUNC && ident->ir_cmd){
            IR* ir = ident->ir_cmd;
            if(debug_plvar){
                print("# %s\n", ident->name);
                print("#\t stack size: %d\n", ident->stack_size);
                for(Ident* it = ident->scope->ident; it; it = it->next){
                    if(it->kind == ID_LVAR){
                        // TODO : スコープは木構造になっていて、子スコープの識別子は今は出せない。
                        // そのためには、子供のスコープを覚えるようにしなければならない
                        dprint_Ident(it, 0);
                    }
                }
            }
            convert_ir2x86asm(ir);
        }
        ident = ident->next;
    }

    print(".Letext0:\n");
}


static void convert_ir2x86asm(IR* ir){
    while(ir){
        switch(ir->cmd){
            case IR_FN_LABEL:
            {
                Ident* func = ir->s1->ident;
                set_section(TEXT);
                print("\t.globl %s\n", func->name);
                print("\t.type	%s, @function\n", func->name);
                print("%s:\n", func->name);

                // file情報を出力
                if(debug_exec){
                    if(func->func_id == 0){
                        func->func_id = func_id++;
                    }
                    print(".LFB%d:\n", func->func_id);

                    if(!func->tok->file->labeled){
                        func->tok->file->labeled = 1;
                        func->tok->file->label = file_label++;
                        print("\t.file %d \"%s\"\n", func->tok->file->label, func->tok->file->path);
                    }
                    print("\t.loc %d %d %d\n", func->tok->file->label,
                                                func->tok->row, func->tok->col);
                }
                push("rbp");
                print("\tmov rbp, rsp\n");
                print("\tsub rsp, %d\n", ((ir->s2->val + 15) / 16) * 16);
                push("r12");
                push("r13");
                push("r14");
                push("r15");
                break;
            }
            case IR_FN_END_LABEL:
            {
                Ident* func = ir->s1->ident;
                print("ret_%s:\n", func->name);
                pop("r15");
                pop("r14");
                pop("r13");
                pop("r12");
                print("\tmov rsp, rbp\n");
                pop("rbp");
                print("\tret\n");
                print(".LFE%d:\n", func->func_id);
                print("\t.size %s, .-%s\n", func->name, func->name);
                break;
            }
            case IR_VA_AREA:
            {
                int label = ir->t->val;
                int gp = ir->s1->val;
                int fp = ir->s2->val;

                // 引数の退避
                print("\tmov QWORD PTR [rbp - 176], rdi\n");
                print("\tmov QWORD PTR [rbp - 168], rsi\n");
                print("\tmov QWORD PTR [rbp - 160], rdx\n");
                print("\tmov QWORD PTR [rbp - 152], rcx\n");
                print("\tmov QWORD PTR [rbp - 144], r8\n");
                print("\tmov QWORD PTR [rbp - 136], r9\n");
                print("\ttest al, al\n");
                print("\tje .L%d\n", label);
                print("\tmovaps XMMWORD PTR [rbp - 128], xmm0\n");
                print("\tmovaps XMMWORD PTR [rbp - 112], xmm1\n");
                print("\tmovaps XMMWORD PTR [rbp - 96], xmm2\n");
                print("\tmovaps XMMWORD PTR [rbp - 80], xmm3\n");
                print("\tmovaps XMMWORD PTR [rbp - 64], xmm4\n");
                print("\tmovaps XMMWORD PTR [rbp - 48], xmm5\n");
                print("\tmovaps XMMWORD PTR [rbp - 32], xmm6\n");
                print("\tmovaps XMMWORD PTR [rbp - 16], xmm7\n");
                print(".L%d:\n", label);
            }
                break;
            case IR_VA_START:
            {
                activateRegLhs(ir->s1);
                print("\tmov DWORD PTR [%s + 0], %d\n", ir->s1->rreg, 8 * ir->s2->val);
                print("\tmov DWORD PTR [%s + 4], %d\n", ir->s1->rreg, 48);
                print("\tlea rax, [rbp + 16]\n");
                print("\tmov QWORD PTR [%s + 8], rax\n", ir->s1->rreg);
                print("\tlea rax, [rbp - 176]\n");
                print("\tmov QWORD PTR [%s + 16], rax\n", ir->s1->rreg);
                break;
            }
            case IR_EXTERN_LABEL:
                print(".globl %s\n", ir->s1->str);
                break;
            case IR_STORE_ARG_REG:
            {
                int size = get_qtype_size(ir->s1->ident->qtype);
                if(size == 1){
                    print("\tmov [rbp - %d], %s\n", ir->s1->ident->offset, argreg8[ir->s2->val]);
                } else if(size == 2){
                    print("\tmov [rbp - %d], %s\n", ir->s1->ident->offset, argreg16[ir->s2->val]);
                } else if(size == 4){
                    print("\tmov [rbp - %d], %s\n", ir->s1->ident->offset, argreg32[ir->s2->val]);
                } else if(size == 8){
                    print("\tmov [rbp - %d], %s\n", ir->s1->ident->offset, argreg64[ir->s2->val]);
                }
                break;
            }
            case IR_LOAD_ARG_REG:
                activateRegLhs(ir->s1);
                print("\tmov %s, %s\n", argreg64[ir->t->val], ir->s1->rreg);
                freeReg(ir->s1);
                break;
            case IR_SET_FLOAT_NUM:
                print("\tmov eax, %d\n", '\0');
                break;
            case IR_RET:
                if(ir->s1){
                    activateRegLhs(ir->s1);
                    print("\tmov rax, %s\n", ir->s1->rreg);
                }
                print("\tjmp ret_%s\n", ir->s2->ident->name);
                break;
            case IR_GVAR_LABEL:
            {
                Ident* ident = ir->s1->ident;
                if(ident->is_string_literal){
                    set_section(DATA);
                    print("%s:\n", ident->name);
                    print("\t.string \"%s\"\n", get_token_string_literal(ident->tok));
                } else if(ident->is_static) {
                    set_section(BSS);
                    print(".L%s:\n", ir->s1->ident->name);
                    print("\t.zero %d\n", ir->s2->val);
                } else {
                    if(ident->reloc){
                        // 初期化あり
                        print("\t.globl\t%s\n", ident->name);
                        set_section(DATA);
                        print("\t.align %d\n", get_qtype_align(ident->qtype));
                        print("\t.type\t%s, @object\n", ident->name);
                        print("\t.size\t%s, %d\n", ident->name, ir->s2->val);
                        print("%s:\n", ir->s1->ident->name);
                        if(!(ident->reloc->label)){
                            switch(ident->reloc->size){
                                case 1:
                                    print("\t.byte %d\n", ident->reloc->data);
                                    break;
                                case 2:
                                    print("\t.value %d\n", ident->reloc->data);
                                    break;
                                case 4:
                                    print("\t.long %d\n", ident->reloc->data);
                                    break;
                                case 8:
                                    print("\t.quad %d\n", ident->reloc->data);
                                    break;
                            }
                        } else {
                            print("\t.quad %s\n", ident->reloc->label);
                        }
                    } else {
                        print("\t.globl\t%s\n", ident->name);
                        set_section(BSS);
                        print("\t.type\t%s, @object\n", ident->name);
                        print("\t.size\t%s, %d\n", ident->name, ir->s2->val);
                        print("%s:\n", ir->s1->ident->name);
                        print("\t.zero %d\n", ir->s2->val);
                    }
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
                print("\tmov rax, %s\n", ir->s1->rreg);
                print("\tcqo\n");
                print("\tidiv %s\n", ir->s2->rreg);
                print("\tmov %s, rax\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_MOD:
                activateRegLhs(ir->s1);

                // idivが受け取るoperandはレジスタなので、
                // 左辺値として割り当てる
                activateRegLhs(ir->s2);
                print("\tmov rax, %s\n", ir->s1->rreg);
                print("\tcqo\n");
                print("\tidiv %s\n", ir->s2->rreg);
                print("\tmov %s, rdx\n", ir->s1->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_EQUAL:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);

                // cmp命令は直値は32bit幅までしか受け取れないので、
                // 左辺値として割り当てる
                activateRegLhs(ir->s2);
                print("\tcmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("\tsete al\n");
                print("\tmovzb %s, al\n", ir->t->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_NOT_EQUAL:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("\tcmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("\tsetne al\n");
                print("\tmovzb %s, al\n", ir->t->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_LT:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("\tcmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("\tsetl al\n");
                print("\tmovzb %s, al\n", ir->t->rreg);
                freeReg(ir->s1);
                freeReg(ir->s2);
                break;
            case IR_LE:
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("\tcmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("\tsetle al\n");
                print("\tmovzb %s, al\n", ir->t->rreg);
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
                    print("\tsal %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                } else {
                    print("\tmov rcx, %s\n", ir->s2->rreg);
                    print("\tsal %s, cl\n", ir->s1->rreg);
                }
                if(ir->s2->kind == REG_IMM){
                    freeRegAll(ir->t, ir->s1, ir->s2);
                }
                break;
            case IR_R_BIT_SHIFT:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->kind == REG_IMM){
                    print("\tsar %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                } else {
                    print("\tmov rcx, %s\n", ir->s2->rreg);
                    print("\tsar %s, cl\n", ir->s1->rreg);
                }
                if(ir->s2->kind == REG_IMM){
                    freeRegAll(ir->t, ir->s1, ir->s2);
                }
                break;
            case IR_ASSIGN:
                activateRegLhs(ir->s1);
                activateRegLhs(ir->s2);
                if(ir->s1->size == 1){
                    print("\tmov [%s], %s\n", ir->s1->rreg, rreg8[ir->s2->idx]);
                } else if(ir->s1->size == 2){
                    print("\tmov [%s], %s\n", ir->s1->rreg, rreg16[ir->s2->idx]);
                } else if(ir->s1->size == 4){
                    print("\tmov [%s], %s\n", ir->s1->rreg, rreg32[ir->s2->idx]);
                } else if(ir->s1->size == 8){
                    print("\tmov [%s], %s\n", ir->s1->rreg, rreg64[ir->s2->idx]);
                }

                if(ir->t){
                    activateRegLhs(ir->t);
                    print("\tmov %s, %s\n", ir->t->rreg, ir->s2->rreg);
                }
                freeReg(ir->s2);
                freeReg(ir->s1);
                break;
            case IR_FN_CALL:
                {
                    if(depth % 2){
                        print("\tsub rsp, 8\n");
                    }
                    print("\tcall %s\n", ir->s1->ident->name);
                    if(depth % 2){
                        print("\tadd rsp, 8\n");
                    }
                    activateRegLhs(ir->t);
                    print("\tmov %s, rax\n", ir->t->rreg);
                }
                break;
            case IR_REL:
                activateRegLhs(ir->t);
                if(ir->s1->ident->kind == ID_LVAR){
                    print("\tlea %s, [rbp - %d]\n", ir->t->rreg, ir->s1->ident->offset);
                } else if(ir->s1->ident->kind == ID_GVAR){
                    if(ir->s1->ident->is_static){
                        print("\tlea %s, [ rip + .L%s ]\n", ir->t->rreg, ir->s1->ident->name);
                    } else {
                        print("\tlea %s, [ rip + %s ]\n", ir->t->rreg, ir->s1->ident->name);
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
                print("\tmov %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_COPY:
            {
                activateRegLhs(ir->t);
                activateRegLhs(ir->s1);
                for(int i = 0; i < ir->s1->size; i++){
                    // r8bレジスタにbyteデータコピー
                    print("\tmov r8b, BYTE PTR [%s + %d]\n", ir->s1->rreg, i);

                    // r8bレジスタのデータをtのレジスタにコピー
                    print("\tmov BYTE PTR [%s + %d], r8b\n", ir->t->rreg, i);
                }
                freeReg(ir->s1);
            }
            case IR_RELEASE_REG_ALL:
                freeRegAllForce();
                break;
            case IR_RELEASE_REG:
                freeReg(ir->t);
                break;
            case IR_LABEL:
                print(".L%d:\n", ir->s1->val);
                break;
            case IR_JNZ:
                activateRegLhs(ir->s1);
                //activateRegRhs(ir->s2);
                print("\tcmp %s, 0\n", ir->s1->rreg);
                print("\tjne .L%d\n", ir->s2->val);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_JZ:
                activateRegLhs(ir->s1);
                //activateRegRhs(ir->s2);
                print("\tcmp %s, 0\n", ir->s1->rreg);
                print("\tje .L%d\n", ir->s2->val);
                freeRegAll(ir->t, ir->s1, ir->s2);
                break;
            case IR_JE:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("\tcmp %s, %s\n", ir->s1->rreg, ir->s2->rreg);
                print("\tje .L%d\n", ir->t->val);
                freeReg(ir->s2);
                break;
            case IR_JMP:
                print("\tjmp .L%d\n", ir->s1->val);
                break;
            case IR_LEA:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                print("\tlea %s, [rbp - %s]\n", ir->s1->rreg, ir->s2->rreg);
                freeReg(ir->s2);
                break;
            case IR_LOAD:
                activateRegLhs(ir->s1);
                activateRegRhs(ir->s2);
                if(ir->s2->is_unsigned){
                    if(ir->s2->size == 1){
                        print("\tmovzx %s, BYTE PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 2){
                        print("\tmovzx %s, WORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 4){
                        print("\tmov %s, DWORD PTR [%s]\n", rreg32[ir->s1->idx], ir->s2->rreg);
                    } else if(ir->s2->size == 8){
                        print("\tmov %s, QWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    }
                } else {
                    if(ir->s2->size == 1){
                        print("\tmovsx %s, BYTE PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 2){
                        print("\tmovsx %s, WORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 4){
                        print("\tmovsxd %s, DWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
                    } else if(ir->s2->size == 8){
                        print("\tmov %s, QWORD PTR [%s]\n", ir->s1->rreg, ir->s2->rreg);
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
                if(debug_exec){
                    if(!ir->s1->tok->file->labeled){
                        ir->s1->tok->file->labeled = 1;
                        ir->s1->tok->file->label = file_label++;
                        print("\t.file %d \"%s\"\n", ir->s1->tok->file->label, ir->s1->tok->file->path);
                    }
                    print("\t.loc %d %d %d\n", ir->s1->tok->file->label,
                                                ir->s1->tok->row, ir->s1->tok->col);
                }
                break;
            case IR_PUSH:
                activateRegLhs(ir->s1);
                push(ir->s1->rreg);
                freeReg(ir->s1);
                break;
            case IR_POP:
                activateRegLhs(ir->s1);
                pop(ir->s1->rreg);
                freeReg(ir->s1);
                break;
            default:
                unreachable();
        }
        ir = ir->next;

        // レジスタの使用状況をクリア
        for(int i = 0; i < 3; i++){
            useReg[i] = -1;
        }
    }
}

static void dprint_Ident(Ident* ident, int level){
    if(ident->kind != ID_LVAR) return;

    print("#\t %s( ofs: %d, size: %d", ident->name, ident->offset, get_qtype_size(ident->qtype));
    if(get_qtype_kind(ident->qtype) == TY_ARRAY){
        print(", len: %d", get_qtype_array_len(ident->qtype));
    }
    print(") : level%d\n", level);
}