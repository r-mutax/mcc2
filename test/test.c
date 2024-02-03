// test function define.
int printf(char* fmt, ...);
int func();
int funcA(int a, int b) {
    return a + b;
}

int main(){
    func();
    funcA(3, 4);
    return 0;
}

int func(){
    return 3;
}