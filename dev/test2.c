#define ASSERT(X, Y)    ((X) == (Y) ? 0 : 1)

extern int a;
int b;

int main(){

    unsigned char uc = 255;
    return ASSERT((char)uc, -11);
}