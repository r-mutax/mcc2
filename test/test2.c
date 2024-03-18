#define TEST_MACRO_2 1
int main(){
    int a = 10;
    #ifndef TEST_MACRO_2
    abcdefg = 10; 
    #else
    a = 20;
    #endif
    return a;
}