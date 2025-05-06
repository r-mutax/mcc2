#include "testinc.h"

int test_extern_int;

void dirty_stack_300(){
    unsigned int byte[256];
    for(int i = 0; i < 256; i++){
        byte[i] = i;
    }
}


int main(){
    printf("mcc2 test...\n\n");

    test_extern_int = 10;
    test_static = 20;

    dirty_stack_300();

    test_expression();
    test_statement();
    test_variable();
    test_type();
    test_primary();
    test_pointer();
    test_assignment();
    test_preprocess();
    test_cast();
    test_function();
    test_varg();
    test_lib();

    ASSERT(test_static, 20);

    printf("test is complete!!!\n");
    return 0;
}
