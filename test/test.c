// macro
#define TEST_MACRO 123

#define ASSERT(X, Y)    ((X) == (Y) ? 0 : exit(1))

// test function define.
int printf(char* fmt, ...);
int exit(int ret);
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
int test_cast();

int g_a;
int g_b;

int main(){
    printf("mcc2 test...\n\n");

    test_expression();
    test_statement();
    test_variable();
    test_type();
    test_primary();
    test_pointer();
    test_assignment();
    test_preprocess();
    test_cast();

    printf("test is complete!!!\n");
    return 0;
}

int test_expression(){
    printf("test of expression..\n");
    ASSERT( 3 + 5, 8);
    ASSERT( 8 - 5, 3);
    ASSERT( 1 + 3 * 4, 13);
    ASSERT( 3 + 4 / 2, 5);
    ASSERT( (9 + 6) / 3, 5);
    ASSERT(5 % 3, 2);
    ASSERT(56 >> 3, 7);
    ASSERT(7 << 3, 56);
    ASSERT(5 & 3, 1);
    ASSERT(5 ^ 3, 6);
    ASSERT(3 | 4, 7);
    ASSERT(8 | 3 ^ 5 & 4, 15);

    printf("test of expression relational..\n");
    ASSERT(3 < 4, 1);
    ASSERT(3 < 2, 0);
    ASSERT(3 <= 3, 1);
    ASSERT(3 <= 4, 1);
    ASSERT(3 <= 2, 0);
    ASSERT(4 > 3, 1);
    ASSERT(4 > 5, 0);
    ASSERT(4 >= 4, 1);
    ASSERT(4 >= 3, 1);
    ASSERT(4 >= 5, 0);

    printf("test of logical expression...\n");
    ASSERT(2 == 2, 1);
    ASSERT(2 == 3, 0);
    ASSERT(3 + 4 != 8, 1);
    ASSERT(3 + 4 != 7, 0);
    ASSERT(3 && 5, 1);
    ASSERT(3 && 0, 0);
    ASSERT(0 && 5, 0);
    ASSERT(0 && 0, 0);
    ASSERT(3 || 5, 1);
    ASSERT(3 || 0, 1);
    ASSERT(0 || 5, 1);
    ASSERT(0 || 0, 0);

    printf("test of comma operator...\n");
    ASSERT(3, (4, 3));
    ASSERT(4, (3, 4));
    ASSERT(5, (3, 4, 5));

    printf("test of condition operator...\n");
    ASSERT(1 ? 6 : 3, 6);
    ASSERT(0 ? 6 : 3, 3);

    printf("test of unary...\n");
    ASSERT(- -10, 10);
    ASSERT(- -+10, 10);
    ASSERT(-10 + 20, 10);
    int u = 0;
    ++u;    // u -> 1
    ASSERT(u, 1);
    ASSERT(u++, 1); // u -> 2
    --u;    // u -> 1
    ASSERT(u, 1);
    ASSERT(--u, 0);

    int pi = 0;
    ASSERT(pi++, 0);
    ASSERT(pi, 1);

    int pd = 0;
    ASSERT(pd--, 0);
    ASSERT(pd, -1);

    int idlist[10];
    for(int i = 0; i < 10; ++i){
        idlist[i] = i;
    }

    int* incp = idlist;
    incp++;
    ASSERT(*incp, 1);
    ASSERT(*(++incp), 2);

    incp--;
    ASSERT(*incp, 1);
    ASSERT(*(--incp), 0);

    return 0;
}

int test_statement(){
    printf("test of while-statement...\n");
    int while_a; while_a = 0;
    while(while_a < 3) while_a = while_a + 1;
    ASSERT(while_a, 3);

    printf("test of do-while-statement...\n");
    int do_while_a; do_while_a = 0;
    do {
        do_while_a = do_while_a + 1;
    } while(do_while_a < 3);
    ASSERT(do_while_a, 3);

    do_while_a = 0;
    do {
        do_while_a = do_while_a + 1;
    } while(0);
    ASSERT(do_while_a, 1);

    printf("test of for-statement...\n");
    int for_a; for_a = 0;
    int for_b; for_b = 0;
    for(for_a = 0; for_a < 5; for_a = for_a + 1) for_b = for_b + 2;
    ASSERT(for_a, 5);
    ASSERT(for_b, 10);

    int for_c = 0;
    for(; for_c < 5;){ for_c += 1; }
    ASSERT(for_c, 5);

    for(int for_d = 0; for_d < 5; ){ for_d += 1; }

    int for_null = 0;
    for(;;){
        for_null += 1;
        if(for_null == 5) break;
    }
    ASSERT(for_null, 5);

    printf("test of if-statement...\n");
    int if_a;
    if_a = 0;
    if(1){
        if_a = 1;
    }
    ASSERT(if_a, 1);

    if(0){
        if_a = 2;
    }
    ASSERT(if_a, 1);

    if(1){
        if_a = 2;
    } else {
        if_a = 20;
    }
    ASSERT(if_a, 2);

    if(0){
        if_a = 2;
    } else {
        if_a = 20;
    }
    ASSERT(if_a, 20);

    if(0){
        if_a = 1;
    } else if(1){
        if_a = 12;
    } else {
        if_a = 123;
    }
    ASSERT(if_a, 12);

    printf("test of return-statement...\n");
    ASSERT(test_return(), 5);

    printf("test of compound-statement..\n");
    int cp; cp = 0;
    int cp2; cp2 = 0;
    int i; i = 0;
    for(i = 0; i < 5; i += 1){
        cp = cp + 1;
        cp2 = cp2 + 2;
    }
    ASSERT(cp, 5);
    ASSERT(cp2, 10);

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
    ASSERT(break_1, 3);

    int break_2 = 0;
    while(1){
        if(break_2 == 4) break;
        break_2 += 1;
    }
    ASSERT(break_2, 4);

    int break_3_1 = 0;
    int break_3_2 = 0;
    for(; break_3_1 < 5; break_3_1 += 1){
        for(break_3_2 = 0; break_3_2 < 5; break_3_2 += 1){
            if(break_3_2 == 3) break;
        }
        ASSERT(break_3_2, 3);
    }
    ASSERT(break_3_1, 5);

    printf("test of continue statement..\n");
    int cont_1 = 0;
    int cont_1_1 = 0;
    for(;cont_1 < 5; cont_1 += 1){
        continue;
        cont_1_1 += 1;
    }
    ASSERT(cont_1, 5);
    ASSERT(cont_1_1, 0);

    int cont_2 = 0;
    int cont_2_1 = 0;
    while(cont_2 < 5){
        cont_2 += 1;
        continue;
        cont_2_1 += 1;
    }
    ASSERT(cont_1, 5);
    ASSERT(cont_1_1, 0);

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
    ASSERT(s2, 5);

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
    ASSERT(s3, 123);

    int s4 = 4;
    switch(s2){
        case 5:
            s3 = 10;
        case 10:
            s4 = 12;
            break;
    }
    ASSERT(s3, 10);
    ASSERT(s4, 12);

    printf("test of label and goto..\n");

    int li = 0;
    LABEL_TEST_HEAD:
    li += 1;
    if(li == 10) goto LABEL_TEST_END;
    goto LABEL_TEST_HEAD;
    LABEL_TEST_END:
    ASSERT(li, 10);

    return 0;
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

    printf("test of assignment..\n");
    return 0;   
}

int test_type(){
    printf("test of char type..\n");
    char c; c = 5;
    ASSERT(c, 5);
    ASSERT(sizeof(c), 1);
    char carr[3];
    ASSERT(sizeof(carr), 3);
    char* cp;
    ASSERT(sizeof(cp), 8);

    printf("test of short type..\n");
    short s; s = 7;
    ASSERT(s, 7);
    ASSERT(sizeof(s), 2);
    short sarr[3];
    ASSERT(sizeof(sarr), 6);
    short* sp;
    ASSERT(sizeof(sp), 8);

    printf("test of int type..\n");
    int i; i = 17;
    ASSERT(i, 17);
    ASSERT(sizeof(i), 4);
    int iarr[3];
    ASSERT(sizeof(iarr), 12);
    int* ip;
    ASSERT(sizeof(ip), 8);

    printf("test of long type..\n");
    long l; l = 17;
    ASSERT(l, 17);
    ASSERT(sizeof(l), 8);
    long larr[3];
    ASSERT(sizeof(larr), 24);
    long* lp;
    ASSERT(sizeof(lp), 8);

    printf("test of struct..\n");
    struct {
        char c;
        short s;
        int i;
    } test_strcut;
    test_strcut.c = 1;
    test_strcut.s = 2;
    test_strcut.i = 3;
    ASSERT(test_strcut.c, 1);
    ASSERT(test_strcut.s, 2);
    ASSERT(test_strcut.i, 3);

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
    ASSERT(test_struct_struct.ss.c, 8);
    ASSERT(test_struct_struct.ss.s, 9);
    ASSERT(test_struct_struct.ss.i, 10);

    struct {
        char c;
        short s;
    } struct_array[10];
    struct_array[5].c = 10;
    struct_array[5].s = 235;
    ASSERT(struct_array[5].c, 10);
    ASSERT(struct_array[5].s, 235);    

    struct abc_struct {
        int a;
        short s;
    } abc_struct_1;
    struct abc_struct abc_struct_2;
    abc_struct_2.a = 10;
    abc_struct_2.s = 34;
    ASSERT(abc_struct_2.a, 10);
    ASSERT(abc_struct_2.s, 34);

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
    ASSERT(test_union.c, 3);
    ASSERT(test_union.s, 3);
    ASSERT(test_union.i, 3);
    ASSERT(sizeof(test_union), 4);

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
    ASSERT(AAA, 0);
    ASSERT(DEF, 200);
    ASSERT(GHI, 201);
    ASSERT(iii_ABC, 100);
    ASSERT(iii_DEF, 200);

    printf("test of type qualifier..\n");
    const int const_a = 0;
    volatile int volatile_b = 0;
    // restrict int restrict_c = 0;
    // restrictキーワードはインテリセンスで怒られるので…
}

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

    return 0;
}

int test_pointer(){
    int data; data = 10;
    int* a; a = &data;
    ASSERT(*a, 10);
    ASSERT(a, &data);
    *a = 15;
    ASSERT(data, 15);
    ASSERT(sizeof a, 8);

    char cd = 10;
    char* cp = &cd;
    ASSERT(*cp, 10);
    ASSERT(cp, &cd);
    *cp = 15;
    ASSERT(cd, 15);
    ASSERT(sizeof cp, 8);

    short sd = 10;
    short* sp = &sd;
    ASSERT(*sp, 10);
    ASSERT(sp, &sd);
    *sp = 15;
    ASSERT(sd, 15);
    ASSERT(sizeof sp, 8);

    long ld = 10;
    long* lp = &ld;
    ASSERT(*lp, 10);
    ASSERT(lp, &ld);
    *lp = 15;
    ASSERT(ld, 15);
    ASSERT(sizeof lp, 8);

    return 0;
}

int test_assignment(){
    printf("test of assignment..\n");
    int a;
    a = 0; a += 3; ASSERT(a, 3);
    a = 0; a -= 3; ASSERT(a, -3);
    a = 2; a *= 3; ASSERT(a, 6);
    a = 8; a /= 4; ASSERT(a, 2);
    a = 13; a %= 5; ASSERT(a, 3);
    a = 56; a >>= 3; ASSERT(a, 7);
    a = 7; a <<= 3; ASSERT(a, 56);
    return 0;
}

#define TEST_MACRO_2 1 + 2
int test_preprocess()
{
    printf("test of preprocess..\n");
    int a = 0;
    a = TEST_MACRO;
    ASSERT(a, 123);
    ASSERT(TEST_MACRO_2, 3);

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
    ASSERT(a, 20);

    int pp_ans = 10;
// test #if directive.
    printf("test of preprocess if directive..\n");
#if 0
    ASSERT(1, 0);
#else

// test Nesting #if directive.
#if 0
    ASSERT(1, 0);
#else
    pp_ans = 10;
#endif
#endif
    ASSERT(pp_ans, 10);

// test #if - #elif - #endif directive.
#if 0
    ASSERT(1, 0);
#elif 0
    ASSERT(77, 0);
#elif 1
    pp_ans = 20;
#endif
    ASSERT(pp_ans, 20);

// test #if directive expression
#if (2 * 3 - 6)
    ASSERT(1, 0);
#endif

#if (4 / 2 + 4) - 6
    ASSERT(1, 0);
#endif

#if (5 % 3 + 1) - 3
    ASSERT(1, 0);
#endif

#if (1 << 3 + 2) - 32
    ASSERT(1, 0);
#endif

#if (8 >> 3 - 2) - 4
    ASSERT(1, 0);
#endif

#if (7 < 4 << 1) - 1
    ASSERT(1, 0);
#endif

#if (2 <= 8 >> 2) - 1
    ASSERT(1, 0);
#endif

#if (17 > 4 << 2) - 1
    ASSERT(1, 0);
#endif

#if (1 >= 4 >> 2) - 1
    ASSERT(1, 0);
#endif

#if (1 == 2 > 0 != 2) - 1
    ASSERT(1, 0);
#endif

#if (2 & 5 ^ 3 | 9) - 11
    ASSERT(1, 0);
#endif

#if (3 == 3 && 4 != 4 ? 3 : 9) - 9
    ASSERT(1, 0);
#endif

#if 0xabc - 2748
    ASSERT(1, 0);
#endif

// TODO : 8進数をトークナイザが読めるようにする
// #if 0567 - 375
//     ASSERT(1, 0);
// #endif


#define TEST_DEF 1
#if defined ( TEST_DEF ) - 1
    ASSERT(1, 0);
#endif

#if ! defined TEST_DEF
    ASSERT(1, 0);
#endif

#ifndef PREDEFINED_MACRO
    ASSERT(1, 0);
#endif

int TEST_CIRC_A = 30;
#define TEST_CIRC_A TEST_CIRC_B
#define TEST_CIRC_B TEST_CIRC_C
#define TEST_CIRC_C TEST_CIRC_A
    TEST_CIRC_A = 50;

#define FUNC_MACRO(X)   X + 1
#define FUNC_MACRO_ARG2(X, Y)   X * Y
    TEST_CIRC_A = FUNC_MACRO(TEST_CIRC_A);
    ASSERT(TEST_CIRC_A, 51);
    TEST_CIRC_A = FUNC_MACRO_ARG2(3, 4);
    ASSERT(TEST_CIRC_A, 12);

#define FUNC_MULTILINE(X)   (X + 1 \
                            + 2)
    ASSERT(FUNC_MULTILINE(3), 6);

    return 0;
}

int test_cast(){
    printf("test of cast..\n");

    char c = 0;
    short s = 0;
    int i = 0;
    long l = 0;
    unsigned char uc = 0;
    unsigned short us = 0;
    unsigned int ui = 0;
    unsigned long ul = 0;

    // char cast to ...
    c = -1;
    ASSERT((short)c, -1);
    ASSERT((int)c, -1);
    ASSERT((long)c, -1);
    ASSERT((unsigned char)c, 255);
    ASSERT((unsigned short)c, 65535);
    ASSERT((unsigned int)c, 4294967295);
    ASSERT((unsigned long)c, 18446744073709551615ul);

    // short cast to ...
    s = -1;
    ASSERT((char)s, -1);
    ASSERT((int)s, -1);
    ASSERT((long)s, -1);
    ASSERT((unsigned char)s, 255);
    ASSERT((unsigned short)s, 65535);
    ASSERT((unsigned int)s, 4294967295);
    ASSERT((unsigned long)s, 18446744073709551615ul);

    // int cast to ...
    i = -1;
    ASSERT((char)i, -1);
    ASSERT((short)i, -1);
    ASSERT((long)i, -1);
    ASSERT((unsigned char)i, 255);
    ASSERT((unsigned short)i, 65535);
    ASSERT((unsigned int)i, 4294967295);
    ASSERT((unsigned long)i, 18446744073709551615ul);

    // long cast to ...
    l = -1;
    ASSERT((char)l, -1);
    ASSERT((short)l, -1);
    ASSERT((int)l, -1);
    ASSERT((unsigned char)l, 255);
    ASSERT((unsigned short)l, 65535);
    ASSERT((unsigned int)l, 4294967295);
    ASSERT((unsigned long)l, 18446744073709551615ul);

    // unsigned char cast to ...
    uc = 255;
    ASSERT((char)uc, -1);
    ASSERT((short)uc, 255);
    ASSERT((int)uc, 255);
    ASSERT((long)uc, 255);
    ASSERT((unsigned short)uc, 255);
    ASSERT((unsigned int)uc, 255);
    ASSERT((unsigned long)uc, 255);

    // unsigned short cast to ...
    us = 65535;
    ASSERT((char)us, -1);
    ASSERT((short)us, -1);
    ASSERT((int)us, 65535);
    ASSERT((long)us, 65535);
    ASSERT((unsigned char)us, 255);
    ASSERT((unsigned int)us, 65535);
    ASSERT((unsigned long)us, 65535);

    // unsigned int cast to ...
    ui = 4294967295;
    ASSERT((char)ui, -1);
    ASSERT((short)ui, -1);
    ASSERT((int)ui, -1);
    ASSERT((long)ui, 4294967295);
    ASSERT((unsigned char)ui, 255);
    ASSERT((unsigned short)ui, 65535);
    ASSERT((unsigned long)ui, 4294967295);

    // unsigned long cast to ...
    ul = 18446744073709551615ul;
    ASSERT((char)ul, -1);
    ASSERT((short)ul, -1);
    ASSERT((int)ul, -1);
    ASSERT((long)ul, -1);
    ASSERT((unsigned char)ul, 255);
    ASSERT((unsigned short)ul, 65535);
    ASSERT((unsigned int)ul, 4294967295);
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
