#include <stdio.h>
#include <stdlib.h>

#include "error.h"
#include "tokenizer.h"
#include "parse.h"
#include "gen_ir.h"
#include "gen_x86_64.h"

int main(int argc, char **argv){
    if( argc != 2){
        fprintf(stderr, "invalid arguments num.\n");
        return 1;
    }

    user_input = argv[1];

    tokenize(argv[1]);
    Function* func = function();

    gen_ir(func);
    gen_x86(get_ir());

    return 0;
}