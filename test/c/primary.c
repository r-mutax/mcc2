#include "testinc.h"

int add(int a, int b);

int test_primary(){
    printf("test of string literal..\n");
    char c;
    c = "abc"[0]; ASSERT(c, 'a');
    c = "abc"[1]; ASSERT(c, 'b');
    c = "abc"[2]; ASSERT(c, 'c');
    c = "abc"[3]; ASSERT(c, 0);
    ASSERT(sizeof "abd", 4);

    printf("test of function call..\n");
    ASSERT(add(3, 4), 7);
    ASSERT(add(5, 6), 11);

    printf("test of character riteral..\n");
    ASSERT('a', 97);

    printf("test of initialize..\n");
    int defini = 10;
    ASSERT(defini, 10);

    unsigned long val = 123;
    ASSERT(val, 123);
    ASSERT(val, 123u);
    ASSERT(val, 123ul);
    ASSERT(val, 123ull);
    ASSERT(val, 123l);
    ASSERT(val, 123lu);
    ASSERT(val, 123llu);

    printf("test of struct..\n");
    struct st_copy {
        int a;
        int b;
    } st1;
    struct st_copy st2;
    st1.a = 1;
    st1.b = 2;
    st2 = st1;
    ASSERT(st2.a, 1);
    ASSERT(st2.b, 2);

    return 0;
}

int add(int a, int b){
    return a + b;
}
