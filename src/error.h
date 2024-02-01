#pragma once
extern char* user_input;
extern char* filename;

void error_at(char* loc, char* fmt, ...);
void error(char* fmt, ...);
void unreachable();
