#include "testinc.h"

int main(){
    printf("mcc2 test...\n\n");

    test_expression();
    test_statement();
    test_variable();
    test_type();
    test_primary();
    test_pointer();
    test_assignment();
    test_preprocess();
    test_cast();

    printf("test is complete!!!\n");
    return 0;
}
