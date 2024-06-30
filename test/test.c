// macro
#define TEST_MACRO 123

// test function define.
int printf(char* fmt, ...);
int exit(int ret);
int assert(int a, int b);
int test_return();
int test_global_variable();
int add(int a, int b);

int test_expression();
int test_statement();
int test_variable();
int test_type();
int test_primary();
int test_pointer();
int test_assignment();
int test_preprocess();

int g_a;
int g_b;

int main(){
    printf("mcc2 test...\n\n");

    test_expression();
    test_statement();
    test_variable();
    test_type();
    test_primary();
    test_preprocess();

    printf("test is complete!!!\n");
    return 0;
}

int test_expression(){
    printf("test of expression..\n");
    assert( 3 + 5, 8);
    assert( 8 - 5, 3);
    assert( 1 + 3 * 4, 13);
    assert( 3 + 4 / 2, 5);
    assert( (9 + 6) / 3, 5);
    assert(5 % 3, 2);
    assert(56 >> 3, 7);
    assert(7 << 3, 56);
    assert(5 & 3, 1);
    assert(5 ^ 3, 6);
    assert(3 | 4, 7);
    assert(8 | 3 ^ 5 & 4, 15);

    printf("test of expression relational..\n");
    assert(3 < 4, 1);
    assert(3 < 2, 0);
    assert(3 <= 3, 1);
    assert(3 <= 4, 1);
    assert(3 <= 2, 0);
    assert(4 > 3, 1);
    assert(4 > 5, 0);
    assert(4 >= 4, 1);
    assert(4 >= 3, 1);
    assert(4 >= 5, 0);

    printf("test of logical expression...\n");
    assert(2 == 2, 1);
    assert(2 == 3, 0);
    assert(3 + 4 != 8, 1);
    assert(3 + 4 != 7, 0);
    assert(3 && 5, 1);
    assert(3 && 0, 0);
    assert(0 && 5, 0);
    assert(0 && 0, 0);
    assert(3 || 5, 1);
    assert(3 || 0, 1);
    assert(0 || 5, 1);
    assert(0 || 0, 0);

    printf("test of comma operator...\n");
    assert(3, (4, 3));
    assert(4, (3, 4));
    assert(5, (3, 4, 5));

    printf("test of condition operator...\n");
    assert(1 ? 6 : 3, 6);
    assert(0 ? 6 : 3, 3);

    printf("test of unary...\n");
    assert(- -10, 10);
    assert(- -+10, 10);
    assert(-10 + 20, 10);
    int u = 0;
    ++u;
    assert(u, 1);
    assert(++u, 2);
    --u;
    assert(u, 1);
    assert(--u, 0);

    int pi = 0;
    assert(pi++, 0);
    assert(pi, 1);

    int pd = 0;
    assert(pd--, 0);
    assert(pd, -1);

    int idlist[10];
    for(int i = 0; i < 10; ++i){
        idlist[i] = i;
    }

    int* incp = idlist;
    incp++;
    assert(*incp, 1);
    assert(*(++incp), 2);
    incp--;
    assert(*incp, 1);
    assert(*(--incp), 0);

    return 0;
}

int test_statement(){
    printf("test of while-statement...\n");
    int while_a; while_a = 0;
    while(while_a < 3) while_a = while_a + 1;
    assert(while_a, 3);

    printf("test of do-while-statement...\n");
    int do_while_a; do_while_a = 0;
    do {
        do_while_a = do_while_a + 1;
    } while(do_while_a < 3);
    assert(do_while_a, 3);

    do_while_a = 0;
    do {
        do_while_a = do_while_a + 1;
    } while(0);
    assert(do_while_a, 1);

    printf("test of for-statement...\n");
    int for_a; for_a = 0;
    int for_b; for_b = 0;
    for(for_a = 0; for_a < 5; for_a = for_a + 1) for_b = for_b + 2;
    assert(for_a, 5);
    assert(for_b, 10);

    int for_c = 0;
    for(; for_c < 5;){ for_c += 1; }
    assert(for_c, 5);

    for(int for_d = 0; for_d < 5; ){ for_d += 1; }

    int for_null = 0;
    for(;;){
        for_null += 1;
        if(for_null == 5) break;
    }
    assert(for_null, 5);

    printf("test of if-statement...\n");
    int if_a;
    if_a = 0;
    if(1){
        if_a = 1;
    }
    assert(if_a, 1);

    if(0){
        if_a = 2;
    }
    assert(if_a, 1);

    if(1){
        if_a = 2;
    } else {
        if_a = 20;
    }
    assert(if_a, 2);

    if(0){
        if_a = 2;
    } else {
        if_a = 20;
    }
    assert(if_a, 20);

    if(0){
        if_a = 1;
    } else if(1){
        if_a = 12;
    } else {
        if_a = 123;
    }
    assert(if_a, 12);

    printf("test of return-statement...\n");
    assert(test_return(), 5);

    printf("test of compound-statement..\n");
    int cp; cp = 0;
    int cp2; cp2 = 0;
    int i; i = 0;
    for(i = 0; i < 5; i += 1){
        cp = cp + 1;
        cp2 = cp2 + 2;
    }
    assert(cp, 5);
    assert(cp2, 10);

    printf("test of void statement..\n");
    ;;;
    {
        ;
    }
    if(1) ;
    if(0) ; else ;

    printf("test of break statement..\n");
    int break_1 = 0;
    for(; break_1 < 5; break_1 += 1){
        if(break_1 == 3){
            break;
        }
    }
    assert(break_1, 3);

    int break_2 = 0;
    while(1){
        if(break_2 == 4) break;
        break_2 += 1;
    }
    assert(break_2, 4);

    int break_3_1 = 0;
    int break_3_2 = 0;
    for(; break_3_1 < 5; break_3_1 += 1){
        for(break_3_2 = 0; break_3_2 < 5; break_3_2 += 1){
            if(break_3_2 == 3) break;
        }
        assert(break_3_2, 3);
    }
    assert(break_3_1, 5);

    printf("test of continue statement..\n");
    int cont_1 = 0;
    int cont_1_1 = 0;
    for(;cont_1 < 5; cont_1 += 1){
        continue;
        cont_1_1 += 1;
    }
    assert(cont_1, 5);
    assert(cont_1_1, 0);

    int cont_2 = 0;
    int cont_2_1 = 0;
    while(cont_2 < 5){
        cont_2 += 1;
        continue;
        cont_2_1 += 1;
    }
    assert(cont_1, 5);
    assert(cont_1_1, 0);

    printf("test of switch statement..\n");
    int s1 = 0;
    int s2 = 3;

    switch(s1){
        case 0:
            s2 = 5;
            break;
        case 3:
            s2 = 4;
            break;
    }
    assert(s2, 5);

    int s3 = 5;
    switch(s2){
        case 0:
            s3 = 51;
            break;
        case 1:
            s3 = 10;
            break;
        default:
            s3 = 123;
            break;
    }
    assert(s3, 123);

    int s4 = 4;
    switch(s2){
        case 5:
            s3 = 10;
        case 10:
            s4 = 12;
            break;
    }
    assert(s3, 10);
    assert(s4, 12);

    printf("test of label and goto..\n");

    int li = 0;
    LABEL_TEST_HEAD:
    li += 1;
    if(li == 10) goto LABEL_TEST_END;
    goto LABEL_TEST_HEAD;
    LABEL_TEST_END:
    assert(li, 10);

    return 0;
}

int test_variable(){
    printf("test of local variable..\n");
    int a; a = 15;
    assert(a, 15);

    int abc; abc = 7;
    assert(abc, 7);

    a = 1;
    a = a + 3;
    assert(a, 4);

    printf("test of global variable..\n");
    g_a = 8;
    assert(g_a, 8);

    test_global_variable();
    assert(g_b, 14);

    printf("test of array..\n");
    int arr[11];
    assert(sizeof(arr), 44);
    *arr = 5;
    assert(*arr, 5);

    *(arr + 2) = 7;
    assert(*(arr + 2), 7);

    arr[3]= 4;
    assert(arr[3], 4);

    printf("test of assignment..\n");
    return 0;   
}

int test_type(){
    printf("test of char type..\n");
    char c; c = 5;
    assert(c, 5);
    assert(sizeof(c), 1);
    char carr[3];
    assert(sizeof(carr), 3);
    char* cp;
    assert(sizeof(cp), 8);

    printf("test of short type..\n");
    short s; s = 7;
    assert(s, 7);
    assert(sizeof(s), 2);
    short sarr[3];
    assert(sizeof(sarr), 6);
    short* sp;
    assert(sizeof(sp), 8);

    printf("test of int type..\n");
    int i; i = 17;
    assert(i, 17);
    assert(sizeof(i), 4);
    int iarr[3];
    assert(sizeof(iarr), 12);
    int* ip;
    assert(sizeof(ip), 8);

    printf("test of long type..\n");
    long l; l = 17;
    assert(l, 17);
    assert(sizeof(l), 8);
    long larr[3];
    assert(sizeof(larr), 24);
    long* lp;
    assert(sizeof(lp), 8);

    printf("test of struct..\n");
    struct {
        char c;
        short s;
        int i;
    } test_strcut;
    test_strcut.c = 1;
    test_strcut.s = 2;
    test_strcut.i = 3;
    assert(test_strcut.c, 1);
    assert(test_strcut.s, 2);
    assert(test_strcut.i, 3);

    printf("test of struct of struct..\n");
    struct {
        struct {
            char c;
            short s;
            int i;
        } ss;
    } test_struct_struct;
    test_struct_struct.ss.c = 8;
    test_struct_struct.ss.s = 9;
    test_struct_struct.ss.i = 10;
    assert(test_struct_struct.ss.c, 8);
    assert(test_struct_struct.ss.s, 9);
    assert(test_struct_struct.ss.i, 10);

    struct {
        char c;
        short s;
    } struct_array[10];
    struct_array[5].c = 10;
    struct_array[5].s = 235;
    assert(struct_array[5].c, 10);
    assert(struct_array[5].s, 235);    

    struct abc_struct {
        int a;
        short s;
    } abc_struct_1;
    struct abc_struct abc_struct_2;
    abc_struct_2.a = 10;
    abc_struct_2.s = 34;
    assert(abc_struct_2.a, 10);
    assert(abc_struct_2.s, 34);

    struct struct_decl{
        int sss;
    };

    printf("test of union..\n");
    union {
        char c;
        short s;
        int i;
    } test_union;
    test_union.c = 1;
    test_union.s = 2;
    test_union.i = 3;
    assert(test_union.c, 3);
    assert(test_union.s, 3);
    assert(test_union.i, 3);
    assert(sizeof(test_union), 4);

    printf("test of enum..\n");
    enum {
        AAA,
        ABC = 100,
        DEF = 200,
        GHI,
    } ddd;

    enum iii{
        iii_ABC = 100,
        iii_DEF = 200,
    } ddd2;
    assert(AAA, 0);
    assert(DEF, 200);
    assert(GHI, 201);
    assert(iii_ABC, 100);
    assert(iii_DEF, 200);

    printf("test of type qualifier..\n");
    const int const_a = 0;
    volatile int volatile_b = 0;
    // restrict int restrict_c = 0;
    // restrictキーワードはインテリセンスで怒られるので…
}

int test_primary(){
    printf("test of string literal..\n");
    char c;
    c = "abc"[0]; assert(c, 'a');
    c = "abc"[1]; assert(c, 'b');
    c = "abc"[2]; assert(c, 'c');
    c = "abc"[3]; assert(c, 0);
    assert(sizeof "abd", 4);

    printf("test of function call..\n");
    assert(add(3, 4), 7);
    assert(add(5, 6), 11);

    printf("test of character riteral..\n");
    assert('a', 97);

    printf("test of initialize..\n");
    int defini = 10;
    assert(defini, 10);
    return 0;
}

int test_pointer(){
    int data; data = 10;
    int* a; a = &data;
    assert(*a, 10);
    assert(a, &data);
    *a = 15;
    assert(data, 15);
    assert(sizeof a, 8);
    return 0;
}

int test_assignment(){
    printf("test of assignment..\n");
    int a;
    a = 0; a += 3; assert(a, 3);
    a = 0; a -= 3; assert(a, -3);
    a = 2; a *= 3; assert(a, 6);
    a = 8; a /= 4; assert(a, 2);
    a = 13; a %= 5; assert(a, 3);
    a = 56; a >>= 3; assert(a, 7);
    a = 7; a <<= 3; assert(a, 56);
    return 0;
}

#define TEST_MACRO_2 1 + 2
int test_preprocess()
{
    printf("test of preprocess..\n");
    int a = 0;
    a = TEST_MACRO;
    assert(a, 123);
    assert(TEST_MACRO_2, 3);

    printf("test of preprocess if-group..\n");
    #ifdef TEST_MACRO_ABCDEFG
    abcdefg = 10;
    #endif

    #ifndef TEST_MACRO_2
    abcdefg = 10; 
    #endif

    #ifndef TEST_MACRO_2
    abcdefg = 16; 
    #else
    a = 20;
    #endif
    assert(a, 20);

    int pp_ans = 10;
// test #if directive.
    printf("test of preprocess if directive..\n");
#if 0
    assert(1, 0);
#else

// test Nesting #if directive.
#if 0
    assert(1, 0);
#else
    pp_ans = 10;
#endif
#endif
    assert(pp_ans, 10);

// test #if - #elif - #endif directive.
#if 0
    assert(1, 0);
#elif 0
    assert(77, 0);
#elif 1
    pp_ans = 20;
#endif
    assert(pp_ans, 20);

// test #if directive expression
#if (2 * 3 - 6)
    assert(1, 0);
#endif

#if (4 / 2 + 4) - 6
    assert(1, 0);
#endif

#if (5 % 3 + 1) - 3
    assert(1, 0);
#endif

#if (1 << 3 + 2) - 32
    assert(1, 0);
#endif

#if (8 >> 3 - 2) - 4
    assert(1, 0);
#endif

#if (7 < 4 << 1) - 1
    assert(1, 0);
#endif

#if (2 <= 8 >> 2) - 1
    assert(1, 0);
#endif

#if (17 > 4 << 2) - 1
    assert(1, 0);
#endif

#if (1 >= 4 >> 2) - 1
    assert(1, 0);
#endif

#if (1 == 2 > 0 != 2) - 1
    assert(1, 0);
#endif

#if (2 & 5 ^ 3 | 9) - 11
    assert(1, 0);
#endif

#if (3 == 3 && 4 != 4 ? 3 : 9) - 9
    assert(1, 0);
#endif

#if 0xabc - 2748
    assert(1, 0);
#endif

// TODO : 8進数をトークナイザが読めるようにする
// #if 0567 - 375
//     assert(1, 0);
// #endif


#define TEST_DEF 1
#if defined ( TEST_DEF ) - 1
    assert(1, 0);
#endif

#if ! defined TEST_DEF
    assert(1, 0);
#endif

#ifndef PREDEFINED_MACRO
    assert(1, 0);
#endif

int TEST_CIRC_A = 30;
#define TEST_CIRC_A TEST_CIRC_B
#define TEST_CIRC_B TEST_CIRC_C
#define TEST_CIRC_C TEST_CIRC_A
    TEST_CIRC_A = 50;

#define FUNC_MACRO(X)   X + 1
#define FUNC_MACRO_ARG2(X, Y)   X * Y
    TEST_CIRC_A = FUNC_MACRO(TEST_CIRC_A);
    assert(TEST_CIRC_A, 51);
    TEST_CIRC_A = FUNC_MACRO_ARG2(3, 4);
    assert(TEST_CIRC_A, 12);

    return 0;
}

int assert(int a, int b){
    if(a != b){
        exit(1);
    }
    return 0;
}

int add(int a, int b){
    return a + b;
}

int test_return(){
    return 5;
    123;
}

int test_global_variable(){
    g_b = 14;
    return 0;
}
