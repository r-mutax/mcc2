char* builtin_def =
    "typedef struct {\n"
    " unsigned int gp_offset; \n"
    " unsigned int fp_offset; \n"
    " void *overflow_arg_area; \n"
    " void *reg_save_area; \n"
    " } __builtin_va_elem; \n"
    "typedef __builtin_va_elem __builtin_va_list[1]; \n"
    "";

