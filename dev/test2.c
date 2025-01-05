#define ASSERT(X, Y)    ((X) == (Y) ? 0 : 1)

extern int a;
int b;

int func(int a, int b){
    int c = 10;
    return a + b;
}

int main(){
    typedef struct {
        int a;
        short s;
    } typedef_struct;
    typedef_struct ts;
    ts.a = 10;
    ts.s = 20;

    unsigned char uc = 255;
    int abc = 1000;
    return ASSERT((char)uc, -11);
}