#pragma once

#include <stdint.h>
#include <stddef.h>
#include <utils.h>
#include <arch/x86_64/idt.h>

void init_pit();
registers_t pit_count(registers_t regs);
void pit_sleep(uint64_t ms);
uint64_t pit_get_ticks();
