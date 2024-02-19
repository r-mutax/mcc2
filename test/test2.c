int printf(char* fmt, ...);
int assert(int a, int b);
int exit(int ret);

int assert(int a, int b){
    if(a != b){
        printf("%d\n", a);
        exit(1);
    }
    return 0;
}

int main(){
    printf("test of char type..\n");
    char c; //c = 5;
    // assert(c, 5);
    // assert(sizeof(c), 1);
    char carr[3];
    // assert(sizeof(carr), 3);
    char* cp;
    // assert(sizeof(cp), 8);

    printf("test of short type..\n");
    short s; s = 7;
    //printf("%d\n", s);
    assert(s, 7);
    printf("test of short type..\n");
    // assert(sizeof(s), 2);
    // short sarr[3];
    // assert(sizeof(sarr), 6);
    // short* sp;
    // assert(sizeof(sp), 8);

    // printf("test of int type..\n");
    // int i; i = 17;
    // assert(i, 17);
    // assert(sizeof(i), 8);
    // int iarr[3];
    // assert(sizeof(iarr), 24);
    // int* ip;
    // assert(sizeof(ip), 8);
    return 0;    
}