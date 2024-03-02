#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>

#include "error.h"
#include "type.h"
#include "tokenizer.h"
#include "parse.h"
#include "gen_ir.h"
#include "gen_x86_64.h"
#include <bits/getopt_core.h>

char* read_file(char* path){

    FILE *fp = fopen(path, "r");
    if(!fp){
        error("invalid file path.");
    }

    if (fseek(fp, 0, SEEK_END) == -1)
        error("%s: fseek: %s", path, strerror(errno));
    size_t size = ftell(fp);
    if (fseek(fp, 0, SEEK_SET) == -1)
        error("%s: fseek: %s", path, strerror(errno));

    char *buf = calloc(1, size + 2);
    fread(buf, size, 1, fp);

    if (size == 0 || buf[size - 1] != '\n')
        buf[size++] = '\n';
    buf[size] = '\0';
    fclose(fp);
    return buf;
}

char* get_filename(char* path){
    char* yen_pos = strrchr(path, '/');
    char* buf = calloc(1, sizeof(strlen(path) + (yen_pos - path) - 1));
    strcpy(buf, yen_pos + 1);
    return buf;
}

void analy_opt(int argc, char** argv){
    int opt;
    while((opt = getopt(argc, argv, "c:o:")) != -1){
        switch(opt){
            case 'c':
                user_input = read_file(optarg);
                filename = get_filename(optarg);
                break;
            case 'o':
                if(optarg != NULL)
                    open_output_file(optarg);
                else{
                    fprintf(stderr, "output file name is not specified.\n");
                    exit(1);
                }
                break;
            default:
                error("invalid option.");
        }
    }
}

int main(int argc, char **argv){

    if(argc > 2){
        analy_opt(argc, argv);
    } else {
        user_input = read_file(argv[1]);
    }
    // initialize
    ty_init();

    // compile
    tokenize(user_input);
    Program();

    gen_ir();
    gen_x86(get_ir());

    close_output_file();

    return 0;
}