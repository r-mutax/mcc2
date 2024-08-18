#include "testinc.h"

int test_return();

int test_statement(){
    printf("test of while-statement...\n");
    int while_a; while_a = 0;
    while(while_a < 3) while_a = while_a + 1;
    ASSERT(while_a, 3);

    printf("test of do-while-statement...\n");
    int do_while_a; do_while_a = 0;
    do {
        do_while_a = do_while_a + 1;
    } while(do_while_a < 3);
    ASSERT(do_while_a, 3);

    do_while_a = 0;
    do {
        do_while_a = do_while_a + 1;
    } while(0);
    ASSERT(do_while_a, 1);

    printf("test of for-statement...\n");
    int for_a; for_a = 0;
    int for_b; for_b = 0;
    for(for_a = 0; for_a < 5; for_a = for_a + 1) for_b = for_b + 2;
    ASSERT(for_a, 5);
    ASSERT(for_b, 10);

    int for_c = 0;
    for(; for_c < 5;){ for_c += 1; }
    ASSERT(for_c, 5);

    for(int for_d = 0; for_d < 5; ){ for_d += 1; }

    int for_null = 0;
    for(;;){
        for_null += 1;
        if(for_null == 5) break;
    }
    ASSERT(for_null, 5);

    printf("test of if-statement...\n");
    int if_a;
    if_a = 0;
    if(1){
        if_a = 1;
    }
    ASSERT(if_a, 1);

    if(0){
        if_a = 2;
    }
    ASSERT(if_a, 1);

    if(1){
        if_a = 2;
    } else {
        if_a = 20;
    }
    ASSERT(if_a, 2);

    if(0){
        if_a = 2;
    } else {
        if_a = 20;
    }
    ASSERT(if_a, 20);

    if(0){
        if_a = 1;
    } else if(1){
        if_a = 12;
    } else {
        if_a = 123;
    }
    ASSERT(if_a, 12);

    printf("test of return-statement...\n");
    ASSERT(test_return(), 5);

    printf("test of compound-statement..\n");
    int cp; cp = 0;
    int cp2; cp2 = 0;
    int i; i = 0;
    for(i = 0; i < 5; i += 1){
        cp = cp + 1;
        cp2 = cp2 + 2;
    }
    ASSERT(cp, 5);
    ASSERT(cp2, 10);

    printf("test of void statement..\n");
    ;;;
    {
        ;
    }
    if(1) ;
    if(0) ; else ;

    printf("test of break statement..\n");
    int break_1 = 0;
    for(; break_1 < 5; break_1 += 1){
        if(break_1 == 3){
            break;
        }
    }
    ASSERT(break_1, 3);

    int break_2 = 0;
    while(1){
        if(break_2 == 4) break;
        break_2 += 1;
    }
    ASSERT(break_2, 4);

    int break_3_1 = 0;
    int break_3_2 = 0;
    for(; break_3_1 < 5; break_3_1 += 1){
        for(break_3_2 = 0; break_3_2 < 5; break_3_2 += 1){
            if(break_3_2 == 3) break;
        }
        ASSERT(break_3_2, 3);
    }
    ASSERT(break_3_1, 5);

    printf("test of continue statement..\n");
    int cont_1 = 0;
    int cont_1_1 = 0;
    for(;cont_1 < 5; cont_1 += 1){
        continue;
        cont_1_1 += 1;
    }
    ASSERT(cont_1, 5);
    ASSERT(cont_1_1, 0);

    int cont_2 = 0;
    int cont_2_1 = 0;
    while(cont_2 < 5){
        cont_2 += 1;
        continue;
        cont_2_1 += 1;
    }
    ASSERT(cont_1, 5);
    ASSERT(cont_1_1, 0);

    printf("test of switch statement..\n");
    int s1 = 0;
    int s2 = 3;

    switch(s1){
        case 0:
            s2 = 5;
            break;
        case 3:
            s2 = 4;
            break;
    }
    ASSERT(s2, 5);

    int s3 = 5;
    switch(s2){
        case 0:
            s3 = 51;
            break;
        case 1:
            s3 = 10;
            break;
        default:
            s3 = 123;
            break;
    }
    ASSERT(s3, 123);

    int s4 = 4;
    switch(s2){
        case 5:
            s3 = 10;
        case 10:
            s4 = 12;
            break;
    }
    ASSERT(s3, 10);
    ASSERT(s4, 12);

    printf("test of label and goto..\n");

    int li = 0;
    LABEL_TEST_HEAD:
    li += 1;
    if(li == 10) goto LABEL_TEST_END;
    goto LABEL_TEST_HEAD;
    LABEL_TEST_END:
    ASSERT(li, 10);

    return 0;
}

int test_return(){
    return 5;
    123;
}
