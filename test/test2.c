#define ASSERT(X, Y)    ((X) == (Y) ? 0 : 1)

int main(){
    char c = -1;
    short s = -1;

    return ASSERT((char)s, -1);
}