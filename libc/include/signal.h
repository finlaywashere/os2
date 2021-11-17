#ifndef SIGNAL_H
#define SIGNAL_H

#include <sys/types.h>
#include <stddef.h>
#include <stdint.h>

#define SIGHUP 1
#define SIGINT 2
#define SIGKILL 3
#define SIGALARM 4

#define HANDLER_ERROR 1

int kill(pid_t pid, int sig);

int register_signal(void *callback, uint64_t index, uint64_t flags, uint64_t condition);

#endif
