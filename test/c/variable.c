#include "testinc.h"

int g_a;
int g_b;
int test_global_variable();

extern int test_extern_int;

int test_variable(){
    printf("test of local variable..\n");
    int a; a = 15;
    ASSERT(a, 15);

    int abc; abc = 7;
    ASSERT(abc, 7);

    a = 1;
    a = a + 3;
    ASSERT(a, 4);

    printf("test of global variable..\n");
    g_a = 8;
    ASSERT(g_a, 8);

    test_global_variable();
    ASSERT(g_b, 14);

    printf("test of array..\n");
    int arr[11];
    ASSERT(sizeof(arr), 44);
    *arr = 5;
    ASSERT(*arr, 5);

    *(arr + 2) = 7;
    ASSERT(*(arr + 2), 7);

    arr[3]= 4;
    ASSERT(arr[3], 4);

    typedef int INT_T;
    int auto a1 = 1;
    signed register int a2 = 2;
    unsigned short restrict a3 = 3;
    signed long volatile int a4 = 34;

    ASSERT(test_extern_int, 10);

    test_static = 30;

    printf("test of assignment..\n");
    return 0;
}

int test_global_variable(){
    g_b = 14;
    return 0;
}
