#pragma once
#include "mcc2.h"

void error_at(Token* tok, char* fmt, ...);
void error(char* fmt, ...);
void unreachable();
