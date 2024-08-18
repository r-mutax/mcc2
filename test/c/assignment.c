#include "testinc.h"

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
