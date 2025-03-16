#include "mcc2.h"
#include "dwarf.h"

static int g_asf = 0;           // .debug_str用のラベル
static Dwarf_dstr* g_dstr;      // .debug_str用の文字列テーブル
static Dwarf_dstr* g_dstr_end;  // .debug_str用の文字列テーブルの最後

static void dwarf_init();       // 初期化
static void dwarf_abbrev();     // .debug_abbrev
static void dwarf_info();       // .debug_info
static void dwarf_str();        // .debug_str
static void dwarf_line();       // .debug_line

static Dwarf_dstr* DWARF_Str(char* fmt, ...);

void dwarf(){
    dwarf_init();

    dwarf_abbrev();
    dwarf_info();
    dwarf_line();
    dwarf_str();
}

static void dwarf_init(){
    // debug_strをつくる
    DWARF_Str(cinfo.working_dir);       // 作業ディレクトリ
    DWARF_Str(cinfo.compile_file);      // コンパイルファイル
    DWARF_Str(cinfo.compiler);          // コンパイラ
}

// .debug_abbrevの出力
static void dwarf_abbrev(){

}

// .debug_infoの出力
static void dwarf_info(){

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
