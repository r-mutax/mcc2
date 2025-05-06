#pragma once

#include "mcc2_lib.h"

#ifdef TESTINC_H
#define DOUBLE_INCLUDE 1
#endif
#define TESTINC_H 1

extern int testinc;

// test utility
#define ASSERT(X, Y)    ((X) == (Y) ? 0 : exit(1))
#define TEST_MACRO 123

static int test_static;
#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif

int printf(char* fmt, ...);
int vprintf ( const char * format, va_list arg );


// test function
int test_expression();
int test_variable();
int test_statement();
int test_type();
int test_primary();
int test_pointer();
int test_assignment();
int test_preprocess();
int test_cast();
int test_function();
int test_varg();
int test_lib();

