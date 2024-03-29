#ifndef PIT_H
#define PIT_H

#include <stdint.h>
#include <stddef.h>
#include <utils.h>
#include <arch/x86_64/idt.h>
#include <process/process.h>

void init_pit();
void pit_count(registers_t* regs);
void pit_sleep(uint64_t ms);
uint64_t pit_get_ticks();

#endif
