// #define MCC
// #include "../src/mcc2.h"
#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif

int printf(const char* format, ...);

int func(){
    static int i = 0;
    static int j = 0;
    printf("i = %d\n", i++);
}

int main(int aa){
    func();
    func();
    func();
    return 0;
}