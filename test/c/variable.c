#include "testinc.h"

int g_a;
int g_b;
int test_global_variable();

char g_init_c = 1;
short g_init_s = 2;
int g_init_i = 3;
long g_init_l = 4;

struct INIT_STRUCT{
    char a;
    short b;
    int c;
    long d;
};

struct INIT_STRUCT g_is1 = { 1, 2, 3, 4 };
struct INIT_STRUCT g_is2 = { .b = 1, 2 };
struct INIT_STRUCT g_is3 = { 1, 2 };
struct INIT_STRUCT g_is4 = { .b = 2, 4, .d = 4 };

static struct INIT_STRUCT g_is5 = { 1, 2 };

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

    printf("test of global variable initialize..\n");
    ASSERT(g_init_c, 1);
    ASSERT(g_init_s, 2);
    ASSERT(g_init_i, 3);
    ASSERT(g_init_l, 4);


    // test of struct initialize
    printf("test of struct initialize..\n");

    struct INIT_STRUCT is = { };
    ASSERT(is.a, 0);
    ASSERT(is.b, 0);
    ASSERT(is.c, 0);
    ASSERT(is.d, 0);

    struct INIT_STRUCT is2 = { 1, 2, 3, 4 };
    ASSERT(is2.a, 1);
    ASSERT(is2.b, 2);
    ASSERT(is2.c, 3);
    ASSERT(is2.d, 4);

    struct INIT_STRUCT is3 = { 1, 2 };
    ASSERT(is3.a, 1);
    ASSERT(is3.b, 2);
    ASSERT(is3.c, 0);
    ASSERT(is3.d, 0);

    struct INIT_STRUCT is4 = { .b = 1, 2 };
    ASSERT(is4.a, 0);
    ASSERT(is4.b, 1);
    ASSERT(is4.c, 2);
    ASSERT(is4.d, 0);

    struct INIT_STRUCT is5 = { 1, .c = 2 };
    ASSERT(is5.a, 1);
    ASSERT(is5.b, 0);
    ASSERT(is5.c, 2);
    ASSERT(is5.d, 0);

    struct INIT_STRUCT is6 = { .b = 2, .d = 4 };
    ASSERT(is6.a, 0);
    ASSERT(is6.b, 2);
    ASSERT(is6.c, 0);
    ASSERT(is6.d, 4);

    printf("test of struct member initialize at global..\n");
    ASSERT(g_is1.a, 1);
    ASSERT(g_is1.b, 2);
    ASSERT(g_is1.c, 3);
    ASSERT(g_is1.d, 4);

    ASSERT(g_is2.a, 0);
    ASSERT(g_is2.b, 1);
    ASSERT(g_is2.c, 2);
    ASSERT(g_is2.d, 0);

    ASSERT(g_is3.a, 1);
    ASSERT(g_is3.b, 2);
    ASSERT(g_is3.c, 0);
    ASSERT(g_is3.d, 0);

    ASSERT(g_is4.a, 0);
    ASSERT(g_is4.b, 2);
    ASSERT(g_is4.c, 4);
    ASSERT(g_is4.d, 4);

    ASSERT(g_is5.a, 1);
    ASSERT(g_is5.b, 2);
    ASSERT(g_is5.c, 0);
    ASSERT(g_is5.d, 0);

    return 0;
}

int test_global_variable(){
    g_b = 14;
    return 0;
}
