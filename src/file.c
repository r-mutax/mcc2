#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "error.h"
#include "mcc2.h"

SrcFile* read_file(char* path){

    fprintf(stderr, path);
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