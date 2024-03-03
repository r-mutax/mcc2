#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

extern char* user_input;
extern char* filename;
extern FILE* fp;

char* strnewcpyn(char* src, int n){
    char* buf = calloc(n, sizeof(char));
    strncpy(buf, src, n);
}

char* format_string(const char* format, ...) {
    // 可変引数の開始
    va_list args;
    va_start(args, format);
    
    // 必要なバッファサイズを計算（終端のヌル文字も考慮）
    int size = vsnprintf(NULL, 0, format, args) + 1;
    va_end(args);
    
    // 必要なメモリを割り当て
    char* buffer = (char*)malloc(size);
    if (buffer == NULL) {
        return NULL; // メモリ割り当て失敗
    }
    
    // 実際に文字列をフォーマットしてバッファに書き込む
    va_start(args, format);
    vsnprintf(buffer, size, format, args);
    va_end(args);
    
    // フォーマット済みの文字列を持つバッファを返す
    return buffer;
}

void printline(char* loc){
    // 行頭を見つける
    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    // 行末を見つける
    char *end = loc;
    while (*end != '\n')
        end++;

    // // 行数を数える
    // int line_num = 1;
    // for (char *p = user_input; p < line; p++){
    //     if (*p == '\n'){
    //         line_num++;
    //     }
    // }

    // エラー箇所を表示する
    // int indent = fprintf(fp, "%s:%d: ", filename, line_num);
    fprintf(fp, "%.*s\n", (int)(end - line), line);

}