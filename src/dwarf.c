#include "mcc2.h"
#include "dwarf.h"

static int g_asf = 0;           // .debug_str用のラベル
static Dwarf_dstr* g_dstr;      // .debug_str用の文字列テーブル

static void dwarf_abbrev();     // .debug_abbrev
static void dwarf_info();       // .debug_info
static void dwarf_str();        // .debug_str
static void dwarf_line();       // .debug_line

static void DWARF_Str(char* fmt, ...);

void dwarf(){
    dwarf_abbrev();
    dwarf_info();
    dwarf_line();
    dwarf_str();
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

}


static void DWARF_Str(char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int size = vsnprintf(NULL, 0, fmt, ap) + 1;
    va_end(ap);

    char* buf = calloc(size, sizeof(char));
    va_start(ap, fmt);
    vsnprintf(buf, size, fmt, ap);
    va_end(ap);

    Dwarf_dstr* dstr = calloc(1, sizeof(Dwarf_dstr));
    dstr->id = g_asf;
    dstr->str = buf;

    print(".LASF%d:\n", g_asf++);
    print("\t.string \"%s\"\n", buf);
}
