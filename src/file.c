#include "mcc2.h"

FILE* fp = NULL;

SrcFile* read_file(const char* path){

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

    char *buf2 = calloc(1, size + 2);

    char *p = buf;
    char *p2 = buf2;
    while(*p){
        if(*p == '\\' && isspace(*(p + 1))){
            // 改行以外のスペースをスキップ
            p++;
            while(*p && *p != '\n' && isspace(*buf))
                buf++;
            // \のつぎが改行ならば
            if(*p == '\n'){
                p++;
                // また改行以外のスペースをスキップ
                while(*p && *p != '\n' && isspace(*p))
                    p++;
                continue;
            }
        }
        *p2++ = *p++;
    }

    SrcFile* file = calloc(1, sizeof(SrcFile));
    file->name = (char*)path;
    file->body = buf2;

    return file;
}

// ファイルパスからディレクトリの文字列を取得する
char* get_dirname(char* path){
    char* yen_pos = strrchr(path, '/');
    char* buf = calloc(1, sizeof(strlen(path) + (yen_pos - path) - 1));
    strncpy(buf, path, yen_pos - path);
    return buf;
}

void file_init()
{
    fp = stdout;
}

void open_output_file(char* filename){
    fp = fopen(filename, "w");
    if(fp == NULL){
        error("cannot open file: %s", filename);
    }
}

void close_output_file(){
    fclose(fp);
}

void print(char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vfprintf(fp, fmt, ap);
    va_end(ap);
}