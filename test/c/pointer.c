#include "testinc.h"

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
