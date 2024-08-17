#define ASSERT(X, Y)    ((X) == (Y) ? 0 : 1)

int main(){

    unsigned char uc = 255;
    return ASSERT((char)uc, -1);
}