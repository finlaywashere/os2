#pragma once

#include <stdint.h>
#include <stddef.h>

void init_paging();
uint64_t* get_page_directory();
void flush_tlb(uint64_t addr);
uint64_t get_physical_addr(uint64_t virtual);

