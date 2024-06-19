#pragma once
#include "mcc2.h"
SrcFile* read_file(const char* filename);
char* get_dirname(char* path);
void file_init();
void print(char* fmt, ...);
void open_output_file(char* filename);
void close_output_file();
