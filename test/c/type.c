#include "testinc.h"

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

    printf("test of struct pointer..\n");
    struct struct_decl* sptr;
    struct struct_decl s;
    sptr = &s;
    sptr->sss = 100;
    ASSERT(sptr->sss, 100);

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
        iii_GHI = 1 + 1,
        iii_JKL = 2 - 1,
        iii_MNO = 3 * 1,
        iii_PQR = 4 / 2,
        iii_STU = 5 % 2,
    } ddd2;
    ASSERT(AAA, 0);
    ASSERT(DEF, 200);
    ASSERT(GHI, 201);
    ASSERT(iii_ABC, 100);
    ASSERT(iii_DEF, 200);
    ASSERT(iii_GHI, 2);
    ASSERT(iii_JKL, 1);
    ASSERT(iii_MNO, 3);
    ASSERT(iii_PQR, 2);
    ASSERT(iii_STU, 1);

    typedef struct {
        int a;
        short s;
    } typedef_struct;
    typedef_struct ts;
    ts.a = 10;
    ts.s = 20;
    ASSERT(ts.a, 10);
    ASSERT(ts.s, 20);

    printf("test of type qualifier..\n");
    const int const_a = 0;
    volatile int volatile_b = 0;
    // restrict int restrict_c = 0;
    // restrictキーワードはインテリセンスで怒られるので…

    printf("test of imcomplete struct..\n");
    typedef struct IMCOMPLETE_STRUCT imcomplete_struct;
    struct IMCOMPLETE_STRUCT {
        int a;
        short s;
    };

    imcomplete_struct ims;
    ims.a = 10;
    ims.s = 20;
    ASSERT(ims.a, 10);

    printf("test of imcomplete enum..\n");
    typedef enum IMCOMPLETE_ENUM imcomplete_enum;
    enum IMCOMPLETE_ENUM {
        A,
        B,
        C,
    };

    imcomplete_enum ime;
    ime = B;
    ASSERT(ime, 1);

    printf("test of _Bool...\n");
    _Bool b = 1;
    ASSERT(b, 1);

    ASSERT(sizeof(b), 1);
    int okng = 0;
    if(b)
        okng = 1;
    ASSERT(okng, 1);

    b = 5;
    ASSERT(b, 1);
    b = 0;
    ASSERT(b, 0);
    b = -1;
    ASSERT(b, 1);

    return 0;
}
