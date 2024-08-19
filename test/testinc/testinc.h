extern int testinc;

// test utility
#define ASSERT(X, Y)    ((X) == (Y) ? 0 : exit(1))
#define TEST_MACRO 123

static int test_static;

int exit(int ret);
int printf(char* fmt, ...);

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
