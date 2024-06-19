#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include "error.h"
#include "type.h"
#include "tokenizer.h"
#include "preprocess.h"
#include "parse.h"
#include "file.h"
#include "semantics.h"
#include "gen_ir.h"
#include "gen_x86_64.h"
#include <bits/getopt_core.h>

static char* filename = NULL;
bool is_preprocess = false;

char* get_filename(char* path){
    char* yen_pos = strrchr(path, '/');
    char* buf = calloc(1, sizeof(strlen(path) + (yen_pos - path) - 1));
    strcpy(buf, yen_pos + 1);
    return buf;
}

void analy_opt(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "c:o:i:d:E")) != -1){
        switch(opt){
            case 'c':
                filename = optarg;
                break;
            case 'i':
                if(optarg != NULL) {
                    add_include_path(optarg);
                } else {
                    fprintf(stderr, "include path is not specified.\n");
                    exit(1);
                }
                break;
            case 'o':
                if(optarg != NULL) {
                    open_output_file(optarg);
                }
                else{
                    fprintf(stderr, "output file name is not specified.\n");
                    exit(1);
                }
                break;
            case 'd':
                if(optarg != NULL) {
                    add_predefine_macro(optarg);
                } else {
                    fprintf(stderr, "macro is not specified.\n");
                    exit(1);
                }
                break;
            case 'E':
                is_preprocess = true;
                break;
            default:
                error("invalid option.");
        }
    }
}

int main(int argc, char **argv){
    // initialize
    ty_init();
    file_init();

    if(argc > 2){
        analy_opt(argc, argv);
    }

    if(filename == NULL){
        fprintf(stderr, "file name is not specified.\n");
        exit(1);
    }

    // get directry name of filename
    char* dir = get_dirname(filename);
    add_include_path(dir);

    // compile
    Token* tok = tokenize(filename);
    if(is_preprocess){
        // プリプロセス出力のオプションが指定されている場合は、
        // トークンをプリプロセスして出力して終了する
        output_token(tok);
        return 0;
    }
    parse(tok);

    // semantics
    semantics();

    // generate
    gen_ir();
    gen_x86(get_ir());

    close_output_file();

    return 0;
}