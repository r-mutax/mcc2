#include "testinc.h"

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

    printf("test of not operator...\n");
    ASSERT(!6, 0);
    ASSERT(!0, 1);

    printf("test of sizeof ...\n");
    ASSERT(sizeof(1), 4);
    ASSERT(sizeof(1), 4);
    int sint = 0;
    ASSERT(sizeof(sint), 4);
    int* sintp = &sint;
    ASSERT(sizeof(sintp), 8);
    ASSERT(sizeof(*sintp), 4);
    ASSERT(sizeof(int), 4);
    ASSERT(sizeof(int*), 8);
    ASSERT(sizeof(int**), 8);
    ASSERT(sizeof(char), 1);
    ASSERT(sizeof(char*), 8);
    ASSERT(sizeof(char**), 8);
    ASSERT(sizeof(short), 2);
    ASSERT(sizeof(long), 8);
    typedef struct {
        short a;
        int b;
    } TestStruct;
    ASSERT(sizeof(TestStruct), 8);
    ASSERT(sizeof(TestStruct*), 8);

    typedef union {
        short a;
        int b;
    } TestUnion;
    ASSERT(sizeof(TestUnion), 4);

    printf("test of full of register ...\n");
    int reg1[3];
    int reg2[3];
    for(int i = 0; i < 3; i++){
        reg1[i] = 3 - i;
    }
    int *preg1 = reg1;
    int *preg2 = reg2;
    for(int i = 0; i < 3; i++){
        *preg2++ = *preg1++;
    }
    ASSERT(reg2[0], 3);
    ASSERT(reg2[1], 2);
    ASSERT(reg2[2], 1);

    return 0;
}
