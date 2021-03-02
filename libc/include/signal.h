#ifndef SIGNAL_H
#define SIGNAL_H

#include <sys/types.h>

#define SIGHUP 1
#define SIGINT 2
#define SIGKILL 3
#define SIGALARM 4

int kill(pid_t pid, int sig);

#endif
