#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "error.h"
#include "mcc2.h"

SrcFile* read_file(char* path){

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

    SrcFile* file = calloc(1, sizeof(SrcFile));
    file->name = path;
    file->body = buf;

    return file;
}

// ファイルパスからディレクトリの文字列を取得する
char* get_dirname(char* path){
    char* yen_pos = strrchr(path, '/');
    char* buf = calloc(1, sizeof(strlen(path) + (yen_pos - path) - 1));
    strncpy(buf, path, yen_pos - path);
    return buf;
}
