#include "mcc2.h"
#include "dwarf.h"
#include <unistd.h>

static int g_asf = 0;           // .debug_str用のラベル
static Dwarf_dstr* g_dstr;      // .debug_str用の文字列テーブル
static Dwarf_dstr* g_dstr_end;  // .debug_str用の文字列テーブルの最後
static Scope* g_scope;          // グローバルスコープ

static int g_abbrev_idx = 1;    // .debug_abbrev用のインデックス

static void dwarf_init();       // 初期化
static void dwarf_abbrev();     // .debug_abbrev
static void dwarf_info();       // .debug_info
static void dwarf_str();        // .debug_str
static void dwarf_line();       // .debug_line

static void dwarf_info_compile_unit();  // コンパイルユニットの情報
static Dwarf_dstr* DWARF_Str(char* fmt, ...);

// abbrev
static void dwarf_abbrev_func(Ident* ident);
static void dwarf_abbrev_param(Parameter* param);
static void dwarf_abbrev_lvar(Ident* lvar);

void dwarf(){
    g_scope = get_global_scope();
    dwarf_init();

    dwarf_abbrev();
    dwarf_info();
    dwarf_line();
    dwarf_str();
}

static void dwarf_init(){
    // debug_strをつくる

    //カレントディレクトリを取得
    char cwd[1024];
    getcwd(cwd, 1024);

    DWARF_Str(cwd);                         // 作業ディレクトリ
    DWARF_Str(cinfo.compile_file->path);    // コンパイルファイル
    DWARF_Str(cinfo.compiler);              // コンパイラ
}

// .debug_abbrevの出力(function)
static void dwarf_abbrev_func(Ident* ident){
    ident->abbrev_idx = g_abbrev_idx;

    DW_ABBREV_IDX();
    DW_ABBREV_TAG(DW_TAG_subprogram);
    if(ident->params || ident->scope->ident){
        DW_CHILDREN_yes();
    } else {
        DW_CHILDREN_no();
    }

    // 外部定義ありか？
    DW_ATTR(DW_AT_external, DW_FORM_flag_present);

    // 名前
    DW_ATTR(DW_AT_name, DW_FORM_strp);

    // 定義されている場所
    DW_ATTR(DW_AT_decl_file, DW_FORM_data1);
    DW_ATTR(DW_AT_decl_line, DW_FORM_data1);
    DW_ATTR(DW_AT_decl_column, DW_FORM_data1);

    // プロトタイプ宣言されているか？
    DW_ATTR(DW_AT_prototyped, DW_FORM_flag_present);

    // 戻り値の型
    DW_ATTR(DW_AT_type, DW_FORM_ref4);

    // 関数の開始アドレス
    DW_ATTR(DW_AT_low_pc, DW_FORM_addr);

    // 関数の終了アドレス
    DW_ATTR(DW_AT_high_pc, DW_FORM_data8);

    // フレームベース
    DW_ATTR(DW_AT_frame_base, DW_FORM_exprloc);

    // 兄弟エントリ
    // 次のエントリのオフセット
    // DW_ATTR(DW_AT_sibling, DW_FORM_ref4);

    DW_ATTR(0x00, 0x00);

    // パラメータ
    for(Parameter* params = ident->params; params != NULL; params = params->next){
        dwarf_abbrev_param(params);
    }

    // 変数
    for(Ident* lvar = ident->scope->ident; lvar != NULL; lvar = lvar->next){
        if(lvar->kind == ID_LVAR && lvar->is_builtin == 0){
            dwarf_abbrev_lvar(lvar);
        }
    }
}

// .debug_abbrevの出力(param)
static void dwarf_abbrev_param(Parameter* param){
    DW_ABBREV_IDX();
    DW_ABBREV_TAG(DW_TAG_formal_parameter);
    DW_CHILDREN_no();

    DW_ATTR(DW_AT_name, DW_FORM_strp);
    DW_ATTR(DW_AT_decl_file, DW_FORM_data1);
    DW_ATTR(DW_AT_decl_line, DW_FORM_data1);
    DW_ATTR(DW_AT_decl_column, DW_FORM_data1);
    DW_ATTR(DW_AT_type, DW_FORM_ref4);
    DW_ATTR(DW_AT_location, DW_FORM_exprloc);
    DW_ATTR(0x00, 0x00);
}

// .debug_abbrevの出力(lvar)
static void dwarf_abbrev_lvar(Ident* lvar){
    DW_ABBREV_IDX();
    DW_ABBREV_TAG(DW_TAG_variable);
    DW_CHILDREN_no();

    DW_ATTR(DW_AT_decl_file, DW_FORM_data1);
    DW_ATTR(DW_AT_decl_line, DW_FORM_data1);
    DW_ATTR(DW_AT_decl_column, DW_FORM_data1);
    DW_ATTR(DW_AT_type, DW_FORM_ref4);
    DW_ATTR(DW_AT_location, DW_FORM_exprloc);
    DW_ATTR(0x00, 0x00);
}

// .debug_abbrevの出力
static void dwarf_abbrev(){
    // セクション宣言とラベル
    print("\t.section\t.debug_abbrev, \"\",@progbits\n");
    print(".Ldebug_abbrev0:\n");

    // 0x01 Compilation Unit
    // コンパイル情報だけはここで出す
    DW_ABBREV_IDX();
    DW_ABBREV_TAG(DW_TAG_compile_unit);
    DW_CHILDREN_no();

    DW_ATTR(DW_AT_producer, DW_FORM_strp);
    DW_ATTR(DW_AT_language, DW_FORM_data1);
    DW_ATTR(DW_AT_name, DW_FORM_strp);
    DW_ATTR(DW_AT_comp_dir, DW_FORM_strp);
    DW_ATTR(DW_AT_low_pc, DW_FORM_addr);
    DW_ATTR(DW_AT_high_pc, DW_FORM_data8);
    DW_ATTR(DW_AT_stmt_list, DW_FORM_sec_offset);
    DW_ATTR(0x00, 0x00);

    // TODO : いろいろだす
    for(Ident* func = g_scope->ident; func != NULL; func = func->next){
        if(func->kind == ID_FUNC){
            dwarf_abbrev_func(func);
        }
    }

    DW_ATTR(0x00, 0x00);
}

// .debug_infoの出力
static void dwarf_info(){
    // セクション宣言とラベル
    print("\t.section\t.debug_info, \"\",@progbits\n");
    print(".Ldebug_info0:\n");

    // debug_infoのヘッダ
    print("\t.long .Ledebug_info0 - .Ldebug_info0 - 4\n");  // Length
    print("\t.short 0x0005\n");                             // DWARF version
    print("\t.byte 0x01\n");                                // UnitType : DW_UT_compile
    print("\t.byte 0x08\n");                                // AddressSize
    print("\t.long 0x00\n");                                // AbbrevOffset

    // 0x01 Compilation Unit
    // コンパイル情報だけはここで出す
    dwarf_info_compile_unit();

    print(".Ledebug_info0:\n");
}

static void dwarf_info_compile_unit(){
    print("\t.uleb128 0x01\n"); // Abbreviation Code;
    print("\t.long .LASF2\n");  // DW_AT_producer
    print("\t.byte 0x01\n");    // DW_AT_language
    print("\t.long .LASF1\n");  // DW_AT_name
    print("\t.long .LASF0\n");  // DW_AT_comp_dir
    print("\t.quad .Ltext0\n"); // DW_AT_low_pc
    print("\t.quad .Letext0-.Ltext0\n"); // DW_AT_high_pc
    print("\t.long 0\n"); // DW_AT_stmt_list
    print("\t.byte 0x00\n"); // 0x00
}

// .debug_lineの出力
static void dwarf_line(){
    // セクション宣言とラベル
    print("\t.section\t.debug_line, \"\",@progbits\n");
    print(".Ldebug_line0:\n");
}

// .debug_strの出力
static void dwarf_str(){
    // セクション宣言
    print("\t.section\t.debug_str, \"MS\",@progbits,1\n");

    for(Dwarf_dstr* dstr = g_dstr; dstr != NULL; dstr = dstr->next){
        print(".LASF%d:\n", dstr->id);
        print("\t.string \"%s\"\n", dstr->str);
    }
}


static Dwarf_dstr* DWARF_Str(char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int size = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    char* buf = calloc(size, sizeof(char));
    va_start(ap, fmt);
    vsnprintf(buf, size, fmt, ap);
    va_end(ap);

    Dwarf_dstr* dstr = calloc(1, sizeof(Dwarf_dstr));
    dstr->id = g_asf++;
    dstr->str = buf;

    if(g_dstr == NULL){
        g_dstr = dstr;
        g_dstr_end = dstr;
    }

    g_dstr_end->next = dstr;
    g_dstr_end = dstr;
}
