// #define MCC
// #include "../src/mcc2.h"
#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif

int printf(const char * format, ...);
int vprintf ( const char * format, va_list arg );

#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

int func(char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
    return 0;
}

int main(){
    int a = 0;
    char* p = "string"
    " string";
    printf("%s\n", p);

    return func("%d %d %d %d\n", 1, 2, 3, 4);
}