#include "testinc.h"

int g_func_a;

int* get_pointer(){
    int* a = &g_func_a;
    *a = 30;
    return a;
}

void void_func(){
    return;
}

int farg1(int a){
    return a * a;
}

int farg2(int a, int b){
    return a + b;
}

static int static_func(){
    return 43;
}

int test_function(){

    printf("test of function call..\n");
    int* p = get_pointer();
    int c;
    c = *p;


    ((*p) == (30) ? 0 : exit(1));

    printf("test of void function call..\n");
    void_func();

    printf("test of function argument..\n");
    ASSERT(farg1(3), 9);
    ASSERT(farg2(3, 4), 7);
    ASSERT(farg2(3, farg1(3)), 12);

    return 0;
}