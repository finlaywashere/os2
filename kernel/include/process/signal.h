#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdint.h>
#include <stddef.h>

int register_signal(uint64_t address, uint64_t index, uint64_t flags, uint64_t condition);
int get_signal(uint64_t flags);
int get_error_signal();

#endif
