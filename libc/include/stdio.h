#ifndef _STDIO_H
#define _STDIO_H
#include <stdarg.h>
#include <stddef.h>
#include <syscall.h>
#include <stdlib.h>
#define SEEK_SET 0
struct file{
	uint64_t id;
};
typedef struct file FILE;
#ifdef __cplusplus
extern "C" {
#endif
extern FILE *stderr, *stdout, *stdin;
#define stderr stderr

#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

int fclose(FILE*);
int fflush(FILE*);
FILE* fopen(const char*, const char*);
int fprintf(FILE*, const char*, ...);
size_t fread(void*, size_t, size_t, FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);
void setbuf(FILE*, char*);
int vfprintf(FILE*, const char*, va_list);
int printf(const char*, ...);
int putchar(int);
#ifdef __cplusplus
}
#endif
#endif
