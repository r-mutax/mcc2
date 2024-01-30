#include "utility.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

char* strnewcpyn(char* src, int n){
    char* buf = calloc(n, sizeof(char));
    strncpy(buf, src, n);
}
