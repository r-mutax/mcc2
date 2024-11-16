#include "testinc.h"

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

// test if directive expression expanding macro
#define TEST_MACRO_EXPAND 2
#if TEST_MACRO_EXPAND - 2
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
