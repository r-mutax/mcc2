#include <stdio.h>
#include <stdlib.h>

#include "tokenizer.h"
#include "parse.h"
#include "gen_ir.h"
#include "gen_x86.h"

int main(int argc, char **argv){
    if( argc != 2){
        fprintf(stderr, "invalid arguments num.\n");
        return 1;
    }

    tokenize(argv[1]);
    Node* node = expr();
    gen_ir(node);
    gen_x86(get_ir());

    return 0;
}