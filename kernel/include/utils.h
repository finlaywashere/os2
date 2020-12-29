#pragma once

#include <stddef.h>
#include <stdint.h>

void memcpy(uint8_t* src, uint8_t* dst, uint64_t count);
uint64_t strlen(char* str);
int memcmp(uint8_t* mem1, uint8_t* mem2, uint64_t count);

