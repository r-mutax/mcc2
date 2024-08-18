#include "mcc2.h"

void warn_tok(Token* tok, char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    char* user_input = tok->file->body;
    char* filename = tok->file->name;
    char* loc = tok->pos;

    // 行頭を見つける
    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    // 行末を見つける
    char *end = loc;
    while (*end != '\n')
        end++;

    // 行数を数える
    int line_num = 1;
    for (char *p = user_input; p < line; p++){
        if (*p == '\n'){
            line_num++;
        }
    }

    // エラー箇所を表示する
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // 空白を表示する
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, " ");

    // エラー箇所を示して、メッセージを表示する
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

void error_tok(Token* tok, char* fmt, ...){
    char* user_input = tok->file->body;
    char* filename = tok->file->name;
    char* loc = tok->pos;

    // 行頭を見つける
    char *line = loc;
    while (user_input < line && line[-1] != '\n')
        line--;

    // 行末を見つける
    char *end = loc;
    while (*end != '\n')
        end++;

    // 行数を数える
    int line_num = 1;
    for (char *p = user_input; p < line; p++){
        if (*p == '\n'){
            line_num++;
        }
    }

    // エラー箇所を表示する
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // 空白を表示する
    int pos = loc - line + indent;
    fprintf(stderr, "%*s", pos, " ");

    // エラー箇所を示して、メッセージを表示する
    fprintf(stderr, "^ ");

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void error_at_src(char* pos, SrcFile* src, char* fmt, ...){
    char* user_input = src->body;
    char* filename = src->name;

    // 行頭を見つける
    char *line = pos;
    while (user_input < line && line[-1] != '\n')
        line--;

    // 行末を見つける
    char *end = pos;
    while (*end != '\n')
        end++;

    // 行数を数える
    int line_num = 1;
    for (char *p = user_input; p < line; p++){
        if (*p == '\n'){
            line_num++;
        }
    }

    // エラー箇所を表示する
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // 空白を表示する
    int pos_ = pos - line + indent;
    fprintf(stderr, "%*s", pos_, " ");

    // エラー箇所を示して、メッセージを表示する
    fprintf(stderr, "^ ");

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void warn_at_src(char* pos, SrcFile* src, char* fmt, ...){
    char* user_input = src->body;
    char* filename = src->name;

    // 行頭を見つける
    char *line = pos;
    while (user_input < line && line[-1] != '\n')
        line--;

    // 行末を見つける
    char *end = pos;
    while (*end != '\n')
        end++;

    // 行数を数える
    int line_num = 1;
    for (char *p = user_input; p < line; p++){
        if (*p == '\n'){
            line_num++;
        }
    }

    // エラー箇所を表示する
    int indent = fprintf(stderr, "%s:%d: ", filename, line_num);
    fprintf(stderr, "%.*s\n", (int)(end - line), line);

    // 空白を表示する
    int pos_ = pos - line + indent;
    fprintf(stderr, "%*s", pos_, " ");

    // エラー箇所を示して、メッセージを表示する
    fprintf(stderr, "^ ");

    va_list ap;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
}

void error(char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);

}
void unreachable()
{
    fprintf(stderr, "%s\n", "Reached unreachable code.");
    exit(1);
}