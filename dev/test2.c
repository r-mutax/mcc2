// #define MCC
// #include "../src/mcc2.h"
#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif

struct INIT_STRUCT{
    char a;
    short b;
    int c;
    long d;
};

int main(int aa){
    struct INIT_STRUCT is = { };

    return 0;
}