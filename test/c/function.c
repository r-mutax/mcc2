#include "testinc.h"

int g_func_a;

int* get_pointer(){
    int* a = &g_func_a;
    *a = 30;
    return a;
}

int test_function(){

    printf("test of function call..\n");
    int* p = get_pointer();
    int c;
    c = *p;


    ((*p) == (30) ? 0 : exit(1));

    return 0;
}