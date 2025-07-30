#ifndef MCC_LIB
#define MCC_LIB 1

typedef void FILE;
#define bool _Bool
#define true 1
#define false 0
#define NULL 0

typedef unsigned long   __kernel_size_t;
typedef __kernel_size_t         size_t;

// stdio.h
#define SEEK_SET 0
#define SEEK_END 2
FILE *fopen(const char *filename, const char *mode);
int fseek(FILE *stream, long int offset, int origin);
int fclose(FILE *stream);
int fprintf(FILE *stream, const char *fmt, ...);
size_t fread(void *buffer, size_t size, size_t count, FILE *stream);
long int ftell(FILE *stream);
int printf(const char *fmt, ...);
int sprintf(char *buffer, const char *format, ...);
extern FILE *stdout;
extern FILE *stderr;

// #define stdout stdout
// #define stderr stderr

// stdlib.h
void *calloc(size_t num, size_t size);
void *realloc(void *ptr, size_t size);
void exit(int status);
void free(void *ptr);
int abs(int n);

// string.h
char *strerror(int errnum);
char *strrchr(const char *string, int c);
size_t strlen(const char *string);
char *strncpy(char *string1, const char *string2, size_t count);
int strcmp(const char *string1, const char *string2);
char *strstr(const char *string1, const char *string2);
char *strcpy(char *string1, const char *string2);
char *strcat(char *string1, const char *string2);
unsigned long int strtoul(const char * string1, char ** string2, int base);
void *memcpy(void *dest, const void *src, size_t count);
void *malloc(size_t size);
int memcmp(const void *buf1, const void *buf2, size_t count);

// errno.h
extern int errno;

// ctype.h
/* excluding space */
int isalpha(int c);
int isspace(int c);
int isdigit(int c);
int toupper(int c);

// stdarg.h
#ifndef _VA_LIST
typedef __builtin_va_list va_list;
#define _VA_LIST
#endif

#define va_start(ap, param) __builtin_va_start(ap, param)
#define va_end(ap)          __builtin_va_end(ap)
#define va_arg(ap, type)    __builtin_va_arg(ap, type)

int vfprintf(FILE *  __restrict__stream, const char *  __restrict__format, va_list arg_ptr);
int vsnprintf(char *fmt, size_t n, const char *format, va_list arg_ptr);

// getopt_core.
extern int getopt (int ___argc, char * *___argv, const char *__shortopts);
extern char* optarg;

// unistd.h
char *getcwd(char *buffer, size_t size);

#endif
