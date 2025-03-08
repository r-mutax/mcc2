#include "mcc2.h"

static int func_id = 1;

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


static void dwarf_info(Scope* global_scope){

    // global_scopeの関数を順番に確認し、func_idを割り当てる
    for(Ident* cur = global_scope->ident; cur; cur = cur->next){
        if(cur->kind == ID_FUNC && cur->funcbody){
            cur->func_id = func_id++;
        }
    }

}

// .fileディレクティブでファイル名を出力する
static void dwarf_file(void){

    int filecount = 0;
    SrcFile** files = get_files(&filecount);
    for(int i = 0; i < filecount; i++){
        print("  .file %d \"%s\"\n", files[i]->label, files[i]->path);
    }
}
static void dwarf_apprev(Scope* global_scope){

    print(".section .debug_abbrev\n");

    //dwarf_apprev_cmpile_unit();

    print("  .uleb128 0x0\n");      // end of apprev
}

int dwarf(Scope* global_scope){

    dwarf_apprev(global_scope);

    dwarf_file();

    dwarf_info(global_scope);

    return -1;
}