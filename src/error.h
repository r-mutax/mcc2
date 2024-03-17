#pragma once
#include "mcc2.h"

void error_tok(Token* tok, char* fmt, ...);
void warn_tok(Token* tok, char* fmt, ...);
void error_at_src(char* pos, SrcFile* src, char* fmt, ...);
void error(char* fmt, ...);
void unreachable();
