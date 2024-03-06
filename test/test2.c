#include "abc.h"
#include "testinc.h"

int main(){
    int a = 10;
    union {
        int i;
        char c;
    } u;
    
    u.i = 10;

    return u.c;
}