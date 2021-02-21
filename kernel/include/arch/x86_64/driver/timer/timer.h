#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>
#include <stddef.h>

// Different timer drivers
#include <arch/x86_64/driver/timer/pit.h>

void init_timers();
uint64_t get_time();
void sleep(uint64_t ms);

#endif
