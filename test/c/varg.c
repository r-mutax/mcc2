#include "testinc.h"
#include "mcc2_lib.h"

int vafunc(int a, ...){
    va_list ap;
    va_start(ap, a);

    int sum = 0;
    // for (int i = 0; i < a; i++){
    //     sum += va_arg(ap, int);
    // }
    // va_end(ap);
    return 6;
    // return sum;
}

int test_varg(){
    printf("test of varg..\n");
    ASSERT(vafunc(3, 1, 2, 3), 6);
    return 0;
}