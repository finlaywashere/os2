#pragma once

#include <stdint.h>
#include <stddef.h>
#include <utils.h>

void init_pit();
void pit_count();
void sleep(uint64_t ms);
