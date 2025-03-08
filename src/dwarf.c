#include "mcc2.h"

static int func_id = 1;
static Scope* global_scope = NULL;


static void dwarf_info(){

    // global_scopeの関数を順番に確認し、func_idを割り当てる
    for(Ident* cur = global_scope->ident; cur; cur = cur->next){
        if(cur->kind == ID_FUNC && cur->funcbody){
            cur->func_id = func_id++;
        }
    }

}

// .debug_lineセクション（file table entry)
static void dwarf_line_ftb(){

    /*
        DWARF5 の file_table_entry
        +--------------------------------------------------------------+
        | ファイル数              : ULEB128                             |
        | ファイルエントリ         : ファイル数分繰り返し                 |
        |   - ファイル名          : NULL終端文字列                       |
        |   - ディレクトリインデックス : ULEB128                         |
        |   - 最終更新時刻        : ULEB128                             |
        |   - ファイルサイズ       : ULEB128                            |
        +--------------------------------------------------------------+
    */

    print("\t.uleb128 1\n");        // file_entry_format_count = 1
    print("\t.uleb128 0x01\n");     // DW_FORM_strp
    print("\t.uleb128 0x08\n");     // DM_FORM_string

    int count = 0;
    SrcFile** files = get_files(&count);
    print("\t.uleb128 %d\n", count);  // ファイル数
    for(int i = 0; i < count; i++){
        SrcFile* file = files[i];
        print("\t.asciz \"%s\"\n", file->path);
        // print("\t.uleb128 %d\n", 1);    // directory index
        // print("\t.uleb128 0\n");        // last modification time
        // print("\t.uleb128 0\n");        // file size
    }
}


// .fileディレクティブでファイル名を出力する
static void dwarf_line_header(void){
    // debug_line header
    /*
    +---------------------------------------------------------------+
    | Unit Length             : 4 バイト                           |
    |    → ヘッダ以降の全体のバイト数（後述のheader_lengthを含む）   |
    +---------------------------------------------------------------+
    | Version                 : 2 バイト                           |
    |    → DWARFのバージョン（例: 5）                                |
    +---------------------------------------------------------------+
    | Header Length           : 4 バイト                           |
    |    → ヘッダ部分全体のバイト数（ディレクトリテーブルと       |
    |       ファイル名テーブルのサイズを含む）                      |
    +---------------------------------------------------------------+
    | Minimum Instruction Length : 1 バイト                        |
    |    → 命令ごとの最小バイト数（x86-64では通常1バイト）         |
    +---------------------------------------------------------------+
    | Default is_stmt         : 1 バイト                           |
    |    → 初期の is_stmt フラグ（通常1: true）                      |
    +---------------------------------------------------------------+
    | Line Base               : 1 バイト (符号付き)                  |
    |    → 特殊オペコードの行番号増分の基準値（例: -5）             |
    +---------------------------------------------------------------+
    | Line Range              : 1 バイト                           |
    |    → 特殊オペコードで表現できる行番号の範囲（例: 14）          |
    +---------------------------------------------------------------+
    | Opcode Base             : 1 バイト                           |
    |    → 標準オペコードの番号の開始値（特殊オペコードとの差別用）   |
    +---------------------------------------------------------------+
    | Standard Opcode Lengths : (Opcode Base - 1) バイトの配列         |
    |    → 各標準オペコードが必要とする追加オペランドの個数           |
    +---------------------------------------------------------------+
    | Include Directories     : NULL終端された文字列群               |
    |    → 例： "." と "src" など（各文字列はASCII、長さは可変）      |
    |    終端は単一のNULLバイト (.byte 0) で示す                     |
    +---------------------------------------------------------------+
    | File Name Table         : 複数のエントリ＋終端                  |
    |    各エントリは以下の構造：                                  |
    |      - ファイル名 : NULL終端文字列（可変長）                  |
    |      - Directory Index : ULEB128 (1バイト以上、値はテーブル内の  |
    |          ディレクトリのインデックス)                           |
    |      - Last Modification Time : ULEB128 (可変長)              |
    |      - File Size : ULEB128 (可変長)                           |
    |    終端エントリは単一のNULL文字（.byte 0）で示す              |
    +---------------------------------------------------------------+

    */

    print(".Ldebug_line_head_start:\n");
    print("\t.long .Ldebug_line_prog_end - .Ldebug_line_head_start - 4\n");
    print("\t.short 5\n");              // DWARF version
    print("\t.byte 8\n");
    print("\t.byte 0\n");
    print(".Ldebug_line_prologue_start:\n");
    print("\t.long .Ldebug_line_head_end - .Ldebug_line_prologue_start\n");
    print("\t.byte 1\n");               // minimum_instruction_length
    print("\t.byte 1\n");               // maximum_ops_per_instruction
    print("\t.byte 1\n");               // default_is_stmt
    print("\t.byte -5\n");              // line_base
    print("\t.byte 14\n");              // line_range
    print("\t.byte 13\n");              // opcode_base(標準オペコード12個)

    // 標準オペコードのオペランドの数(12個)
    print("\t.byte 0\n");   // 01 DW_LNS_copy op()
    print("\t.byte 1\n");   // 02 DW_LNS_advance_pc(uleb128)
    print("\t.byte 1\n");   // 03 DW_LNS_advance_line(sleb128)
    print("\t.byte 1\n");   // 04 DW_LNS_set_file(uleb128)
    print("\t.byte 1\n");   // 05 DW_LNS_set_column(uleb128)
    print("\t.byte 0\n");   // 06 DW_LNS_negate_stmt()
    print("\t.byte 0\n");   // 07 DW_LNS_set_basic_block()
    print("\t.byte 0\n");   // 08 DW_LNS_const_add_pc()
    print("\t.byte 1\n");   // 09 DW_LNS_fixed_advance_pc(uhalf) ★
    print("\t.byte 0\n");   // 10 DW_LNS_set_prologue_end()
    print("\t.byte 0\n");   // 11 DW_LNS_set_epilogue_begin()
    print("\t.byte 1\n");   // 12 DW_LNS_set_isa(uleb128)

    // includeディレクトリ
    print("\t.uleb128 1\n");    // directory_entry_format_count = 1
    print("\t.uleb128 0x01\n");    // DW_FORM_strp
    print("\t.uleb128 0x08\n"); // DM_FORM_string
    IncludePath* inc_path = get_include_paths();
    // まず数を数えておく
    int cnt = 0;
    for(IncludePath* cur = inc_path; cur; cur = cur->next){
        cnt++;
    }
    print("\t.uleb128 %d\n", cnt);

    for(; inc_path; inc_path = inc_path->next){
        printf("%s\n", inc_path->path);
        print("\t.asciz \"%s\"\n", inc_path->path);
    }

    // file name tableエントリ
    dwarf_line_ftb();

    print(".Ldebug_line_head_end:\n");
}

// .debug_lineの行プログラムを出力する
static void dwarf_line_programs(){
    // debug_line prog
    print(".Ldebug_line_prog_start:\n");
    print("\t.byte 0x00\n");
    print("\t.uleb128 0x01\n");
    print("\t.byte 0x01\n");
    print(".Ldebug_line_prog_end:\n");
}

static void dwarf_apprev_cmpile_unit(){
    // compile_unit
    print(".Ldebug_abbrev0:\n");
    print("  .byte 0x01\n");        // apprev code (DW_TAG_compile_unit)
    print("  .uleb128 0x11\n");     // DW_TAG_compile_unit
    print("  .byte 0x01\n");        // DW_CHILDREN_yes
    print("  .uleb128 0x25\n");     // DW_AT_producer   1
    print("  .uleb128 0x0e\n");     // DW_FORM_strp
    print("  .uleb128 0x13\n");     // DW_AT_language   2
    print("  .uleb128 0x0b\n");     // DW_FORM_data1
    print("  .uleb128 0x03\n");     // DW_AT_name       3
    print("  .uleb128 0x0e\n");     // DW_FORM_strp
    print("  .uleb128 0x1b\n");     // DW_AT_comp_dir   4
    print("  .uleb128 0x08\n");     // DW_FORM_string
    print("  .uleb128 0x10\n");     // DW_AT_stmt_list  5
    print("  .uleb128 0x17\n");     // DW_FORM_sec_offset
    print("  .uleb128 0x0\n");      // end of entry
    print("  .uleb128 0x0\n");      // end of entry
}

// a.debug_pprevセクションの出力
static void dwarf_apprev(){

    print(".section .debug_abbrev\n");

    //dwarf_apprev_cmpile_unit();

    print("  .uleb128 0x0\n");      // end of apprev
}

// .debug_lineセクションの出力
static void dwarf_line(){

    print(".section .debug_line\n");

    dwarf_line_header();

    dwarf_line_programs();
}

int dwarf(Scope* scope){
    global_scope = scope;

    dwarf_apprev();

    dwarf_line();

    dwarf_info();

    return -1;
}