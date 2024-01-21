#include "error.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

char* user_input;

void error_at(char* loc, char* fmt, ...){
    va_list ap;
    va_start(ap, fmt);

    int pos = loc - user_input;
    fprintf(stderr, "%s\n", user_input);
    fprintf(stderr, "%*s", pos, " ");
    fprintf(stderr, "^ ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    exit(1);
}

void unreachable()
{
    fprintf(stderr, "%s\n", "Reached unreachable code.");
    exit(1);
}