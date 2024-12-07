#include "testinc.h"

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

    // pointer cast to ...
    printf("test of pointer cast..\n");
    i = 5555;
    int* pi = &i;
    ASSERT(*(char*)pi, -77);

    typedef struct {
        char a;
        char b;
        char c;
        char d;
    } PointerCastStruct;
    PointerCastStruct* pcs;
    long a = 123456;
    ASSERT(((PointerCastStruct*)&a)->a, 64);

    PointerCastStruct pcs2;
    pcs2.a = 1;
    pcs2.b = 12;
    typedef struct {
        char a;
        char b;
    } PointerCastStruct2;
    (PointerCastStruct2*)&pcs2;
    ASSERT(((PointerCastStruct2*)&pcs2)->b, 12);
}
