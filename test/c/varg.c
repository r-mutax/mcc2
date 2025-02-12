#include "testinc.h"

int vafunc(int a, ...){
    va_list ap;
    va_start(ap, a);

    int sum = 0;
    for (int i = 0; i < a; i++){
        sum += va_arg(ap, int);
    }
    va_end(ap);
    return sum;
}

void vafuncstr(char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);
    vprintf(fmt, ap);
    va_end(ap);
}

int test_varg(){
    printf("test of varg..\n");
    ASSERT(vafunc(3, 1, 2, 3), 6);

    printf("test of varg with string..\n");
    vafuncstr("%d %d %d %d\n", 1, 2, 3, 4);
    vafuncstr("%s %s %s %s\n", "a", "b", "c", "d");
    return 0;
}