// #define MCC
// #include "../src/mcc2.h"
#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif

int a = 40;

int main(int aa){
    return a;
}