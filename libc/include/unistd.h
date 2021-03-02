#ifndef _UNISTD_H
#define _UNISTD_H
#include <sys/types.h>
#include <syscall.h>
#ifdef __cplusplus
extern "C" {
#endif
int execv(const char*, char* const[]);
int execve(const char*, char* const[], char* const[]);
int execvp(const char*, char* const[]);
pid_t fork(void);
uid_t getuid(void);
gid_t getgid(void);
pid_t getpid(void);
int setuid(uid_t);
int setgid(gid_t);
#ifdef __cplusplus
}
#endif
#endif
