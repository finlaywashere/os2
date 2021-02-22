#ifndef SAFETY_H
#define SAFETY_H

#include <stdint.h>
#include <stddef.h>

#define CUTOFF_MEM 0xffff000000000000

int usermode_buffer_safety(uint64_t addr, uint64_t size);
int value_safety(uint64_t value, uint64_t min, uint64_t max);

#endif
