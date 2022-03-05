#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>
#include <stddef.h>

#define SIGNAL_SIGHUP 1
#define SIGNAL_SIGINT 2
#define SIGNAL_SIGKILL 3
#define SIGNAL_SIGALARM 4
#define SIGNAL_SIGTERM 5
#define SIGNAL_SIGTRAP 6
#define SIGNAL_SIGABRT 7

int register_signal(uint64_t address, uint64_t index, uint64_t flags, uint64_t condition);
int get_signal(uint64_t flags);
int get_error_signal();

#endif
