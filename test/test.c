// test function define.
int printf(char* fmt, ...);
int exit(int ret);
int assert(int a, int b);
int test_return();
int test_global_variable();
int add(int a, int b);

int test_expression();
int test_statement();
int test_variable();
int test_type();
int test_primary();
int test_pointer();
int test_assignment();

int g_a;
int g_b;

int main(){
    printf("mcc2 test...\n\n");

    test_expression();
    test_statement();
    test_variable();
    test_type();
    test_primary();

    printf("test is complete!!!\n");
    return 0;
}

int test_expression(){
    printf("test of expression..\n");
    assert( 3 + 5, 8);
    assert( 8 - 5, 3);
    assert( 1 + 3 * 4, 13);
    assert( 3 + 4 / 2, 5);
    assert( (9 + 6) / 3, 5);
    assert(5 % 3, 2);
    assert(56 >> 3, 7);
    assert(7 << 3, 56);
    assert(5 & 3, 1);
    assert(5 ^ 3, 6);
    assert(3 | 4, 7);
    assert(8 | 3 ^ 5 & 4, 15);

    printf("test of expression relational..\n");
    assert(3 < 4, 1);
    assert(3 < 2, 0);
    assert(3 <= 3, 1);
    assert(3 <= 4, 1);
    assert(3 <= 2, 0);
    assert(4 > 3, 1);
    assert(4 > 5, 0);
    assert(4 >= 4, 1);
    assert(4 >= 3, 1);
    assert(4 >= 5, 0);

    printf("test of logical expression...\n");
    assert(2 == 2, 1);
    assert(2 == 3, 0);
    assert(3 + 4 != 8, 1);
    assert(3 + 4 != 7, 0);
    assert(3 && 5, 1);
    assert(3 && 0, 0);
    assert(0 && 5, 0);
    assert(0 && 0, 0);
    assert(3 || 5, 1);
    assert(3 || 0, 1);
    assert(0 || 5, 1);
    assert(0 || 0, 0);

    printf("test of ternary operator...\n");
    assert(1 ? 6 : 3, 6);
    assert(0 ? 6 : 3, 3);

    printf("test of unary...\n");
    assert(- -10, 10);
    assert(- -+10, 10);
    assert(-10 + 20, 10);
}

int test_statement(){
    printf("test of while-statement...\n");
    int while_a; while_a = 0;    
    while(while_a < 3) while_a = while_a + 1;
    assert(while_a, 3);

    printf("test of for-statement...\n");
    int for_a; for_a = 0;
    int for_b; for_b = 0;
    for(for_a = 0; for_a < 5; for_a = for_a + 1) for_b = for_b + 2;
    assert(for_a, 5);
    assert(for_b, 10);

    int for_c = 0;
    for(; for_c < 5;){ for_c += 1; }
    assert(for_c, 5);

    for(int for_d = 0; for_d < 5; ){ for_d += 1; }

    printf("test of if-statement...\n");
    int if_a;
    if_a = 0;
    if(1){
        if_a = 1;
    }
    assert(if_a, 1);

    if(0){
        if_a = 2;
    }
    assert(if_a, 1);

    if(1){
        if_a = 2;
    } else {
        if_a = 20;
    }
    assert(if_a, 2);

    if(0){
        if_a = 2;
    } else {
        if_a = 20;
    }
    assert(if_a, 20);

    if(0){
        if_a = 1;
    } else if(1){
        if_a = 12;
    } else {
        if_a = 123;
    }
    assert(if_a, 12);

    printf("test of return-statement...\n");
    assert(test_return(), 5);

    printf("test of compound-statement..\n");
    int cp; cp = 0;
    int cp2; cp2 = 0;
    int i; i = 0;
    for(i = 0; i < 5; i += 1){
        cp = cp + 1;
        cp2 = cp2 + 2;
    }
    assert(cp, 5);
    assert(cp2, 10);

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
    assert(break_1, 3);

    int break_2 = 0;
    while(1){
        if(break_2 == 4) break;
        break_2 += 1;
    }
    assert(break_2, 4);

    int break_3_1 = 0;
    int break_3_2 = 0;
    for(; break_3_1 < 5; break_3_1 += 1){
        for(break_3_2 = 0; break_3_2 < 5; break_3_2 += 1){
            if(break_3_2 == 3) break;
        }
        assert(break_3_2, 3);
    }
    assert(break_3_1, 5);

    printf("test of continue statement..\n");
    int cont_1 = 0;
    int cont_1_1 = 0;
    for(;cont_1 < 5; cont_1 += 1){
        continue;
        cont_1_1 += 1;
    }
    assert(cont_1, 5);
    assert(cont_1_1, 0);

    int cont_2 = 0;
    int cont_2_1 = 0;
    while(cont_2 < 5){
        cont_2 += 1;
        continue;
        cont_2_1 += 1;
    }
    assert(cont_1, 5);
    assert(cont_1_1, 0);

}

int test_variable(){
    printf("test of local variable..\n");
    int a; a = 15;
    assert(a, 15);

    int abc; abc = 7;
    assert(abc, 7);

    a = 1;
    a = a + 3;
    assert(a, 4);

    printf("test of global variable..\n");
    g_a = 8;
    assert(g_a, 8);

    test_global_variable();
    assert(g_b, 14);

    printf("test of array..\n");
    int arr[11];
    assert(sizeof(arr), 88);
    *arr = 5;
    assert(*arr, 5);

    *(arr + 2) = 7;
    assert(*(arr + 2), 7);

    arr[3]= 4;
    assert(arr[3], 4);

    printf("test of assignment..\n");
    
}

int test_type(){
    printf("test of char type..\n");
    char c; c = 5;
    assert(c, 5);
    assert(sizeof(c), 1);
    char carr[3];
    assert(sizeof(carr), 3);
    char* cp;
    assert(sizeof(cp), 8);

    printf("test of short type..\n");
    short s; s = 7;
    assert(s, 7);
    assert(sizeof(s), 2);
    short sarr[3];
    assert(sizeof(sarr), 6);
    short* sp;
    assert(sizeof(sp), 8);

    printf("test of int type..\n");
    int i; i = 17;
    assert(i, 17);
    assert(sizeof(i), 8);
    int iarr[3];
    assert(sizeof(iarr), 24);
    int* ip;
    assert(sizeof(ip), 8);

}

int test_primary(){
    printf("test of string literal..\n");
    char c;
    c = "abc"[0]; assert(c, 'a');
    c = "abc"[1]; assert(c, 'b');
    c = "abc"[2]; assert(c, 'c');
    c = "abc"[3]; assert(c, 0);
    assert(sizeof "abd", 4);

    printf("test of function call..\n");
    assert(add(3, 4), 7);
    assert(add(5, 6), 11);

    printf("test of character riteral..\n");
    assert('a', 97);

    printf("test of initialize..\n");
    int defini = 10;
    assert(defini, 10);
}

int test_pointer(){
    int data; data = 10;
    int* a; a = &data;
    assert(*a, 10);
    assert(a, &data);
    *a = 15;
    assert(data, 15);
    assert(sizeof a, 8);
}

int test_assignment(){
    printf("test of assignment..\n");
    int a;
    a = 0; a += 3; assert(a, 3);
    a = 0; a -= 3; assert(a, -3);
    a = 2; a *= 3; assert(a, 6);
    a = 8; a /= 4; assert(a, 2);
    a = 13; a %= 5; assert(a, 3);
    a = 56; a >>= 3; assert(a, 7);
    a = 7; a <<= 3; assert(a, 56);
}

int assert(int a, int b){
    if(a != b){
        exit(1);
    }
    return 0;
}

int add(int a, int b){
    return a + b;
}

int test_return(){
    return 5;
    123;
}

int test_global_variable(){
    g_b = 14;
    return 0;
}
