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
    int a = 9;
    switch(a){
        case 1:
            a = 10;
            break;
        case 2:
            a = 11;
            break;
    }
    return 0;    
}