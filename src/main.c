#include "mcc2.h"

static char* filename = NULL;
bool is_preprocess = false;
CompilationInfo cinfo;

// デバッグモードを有効化する関数
void enable_debug_mode(const char *mode) {
    if (strcmp(mode, "register") == 0) {
        debug_regis = 1;
    } else if (strcmp(mode, "plvar") == 0){
        debug_plvar = 1;
    } else {
        fprintf(stderr, "Unknown debug mode: %s\n", mode);
        exit(1); // 不明なモードの場合は終了
    }
}

void analy_opt(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "c:o:I:d:x:Eg")) != -1){
        switch(opt){
            case 'c':
                filename = optarg;
                cinfo.working_dir = get_dirname(optarg);
                break;
            case 'I':
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
            case 'x':
                enable_debug_mode(optarg);
                break;
            case 'E':
                is_preprocess = true;
                break;
            case 'g':
                debug_exec = 1;
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
    init_preprocess();
    cinfo.compiler = "mcc2";
    cinfo.global_scope = get_global_scope();

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

    // add standard include path
    add_std_include_path("/usr/local/include");
    add_std_include_path("/usr/include/x86_64-linux-gnu");
    add_std_include_path("/usr/lib/gcc/x86_64-linux-gnu/13/include");
    add_std_include_path("/usr/include");

    // builtin def scan
    Token* tok = tokenize_string(builtin_def);

    // compile
    get_token_before_eof(tok)->next = tokenize(filename);
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
    gen_x86();

    if(debug_exec){
       dwarf();
    }

    close_output_file();

    return 0;
}