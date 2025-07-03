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

int test_static_var_in_func(){
    static int a = 2;
    a++;
    {
        int b = 0;
        {
            int c = 0;
        }
    }
    printf("test_static_var_in_func: %d\n", a);
    return a;
}

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
    printf("222");

    int arr_arr[3][2];
    arr_arr[0][0] = 1;
    arr_arr[0][1] = 2;
    arr_arr[1][0] = 3;
    arr_arr[1][1] = 4;
    arr_arr[2][0] = 5;
    arr_arr[2][1] = 6;
    printf("222");
    ASSERT(arr_arr[0][0], 1);
    ASSERT(arr_arr[0][1], 2);
    ASSERT(arr_arr[1][0], 3);
    ASSERT(arr_arr[1][1], 4);
    ASSERT(arr_arr[2][0], 5);
    ASSERT(arr_arr[2][1], 6);
    ASSERT(arr_arr[1][0] + arr_arr[2][1], 9);

    int arr_arr_arr[2][3][4];
    arr_arr_arr[0][0][0] = 1;
    arr_arr_arr[0][0][1] = 2;
    arr_arr_arr[0][0][2] = 3;
    arr_arr_arr[0][0][3] = 4;
    arr_arr_arr[0][1][0] = 5;
    arr_arr_arr[0][1][1] = 6;
    arr_arr_arr[0][1][2] = 7;
    arr_arr_arr[0][1][3] = 8;
    arr_arr_arr[0][2][0] = 9;
    arr_arr_arr[0][2][1] = 10;
    arr_arr_arr[0][2][2] = 11;
    arr_arr_arr[0][2][3] = 12;
    arr_arr_arr[1][0][0] = 13;
    arr_arr_arr[1][0][1] = 14;
    arr_arr_arr[1][0][2] = 15;
    arr_arr_arr[1][0][3] = 16;
    arr_arr_arr[1][1][0] = 17;
    arr_arr_arr[1][1][1] = 18;
    arr_arr_arr[1][1][2] = 19;
    arr_arr_arr[1][1][3] = 20;
    arr_arr_arr[1][2][0] = 21;
    arr_arr_arr[1][2][1] = 22;
    arr_arr_arr[1][2][2] = 23;
    arr_arr_arr[1][2][3] = 24;
    ASSERT(arr_arr_arr[0][0][0], 1);
    ASSERT(arr_arr_arr[0][0][1], 2);
    ASSERT(arr_arr_arr[0][0][2], 3);
    ASSERT(arr_arr_arr[0][0][3], 4);
    ASSERT(arr_arr_arr[0][1][0], 5);
    ASSERT(arr_arr_arr[0][1][1], 6);
    ASSERT(arr_arr_arr[0][1][2], 7);
    ASSERT(arr_arr_arr[0][1][3], 8);
    ASSERT(arr_arr_arr[0][2][0], 9);
    ASSERT(arr_arr_arr[0][2][1], 10);
    ASSERT(arr_arr_arr[0][2][2], 11);
    ASSERT(arr_arr_arr[0][2][3], 12);
    ASSERT(arr_arr_arr[1][0][0], 13);
    ASSERT(arr_arr_arr[1][0][1], 14);
    ASSERT(arr_arr_arr[1][0][2], 15);
    ASSERT(arr_arr_arr[1][0][3], 16);
    ASSERT(arr_arr_arr[1][1][0], 17);
    ASSERT(arr_arr_arr[1][1][1], 18);
    ASSERT(arr_arr_arr[1][1][2], 19);
    ASSERT(arr_arr_arr[1][1][3], 20);
    ASSERT(arr_arr_arr[1][2][0], 21);
    ASSERT(arr_arr_arr[1][2][1], 22);
    ASSERT(arr_arr_arr[1][2][2], 23);
    ASSERT(arr_arr_arr[1][2][3], 24);


    typedef int INT_T;
    int auto a1 = 1;
    signed register int a2 = 2;
    unsigned short restrict a3 = 3;
    signed long volatile int a4 = 34;

    ASSERT(test_extern_int, 10);

    test_static = 30;

    printf("test of static variable in function..\n");
    ASSERT(test_static_var_in_func(), 3);
    ASSERT(test_static_var_in_func(), 4);
    ASSERT(test_static_var_in_func(), 5);

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

    printf("test of array initialize...\n");
    int arr_init[2] = { 5, 9 };
    ASSERT(arr_init[0], 5);
    ASSERT(arr_init[1], 9);

    return 0;
}

int test_global_variable(){
    g_b = 14;
    return 0;
}
