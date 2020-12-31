#pragma once

#include <stdint.h>
#include <stddef.h>

void init_pit();
void pit_count();
void sleep(uint64_t ms);
