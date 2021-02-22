#ifndef _STDLIB_H
#define _STDLIB_H
#ifdef __cplusplus
extern "C" {
#endif
#include <stddef.h>
#include <stdint.h>

#define PROT_EXEC 1
#define PROT_READ 2
#define PROT_WRITE 4
#define PROT_NONE 8

#define MAP_ANONYMOUS 1

void mmap(void*,size_t,int,int,int,int);
void abort(void);
int atexit(void (*)(void));
int atoi(const char*);
void free(void*);
char* getenv(const char*);
void* malloc(size_t);
void exit(int);
#ifdef __cplusplus
}
#endif
#endif
