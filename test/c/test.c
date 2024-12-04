#include "testinc.h"

int test_extern_int;

int main(){
    printf("mcc2 test...\n\n");

    test_extern_int = 10;
    test_static = 20;

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

    ASSERT(test_static, 20);

    printf("test is complete!!!\n");
    return 0;
}
