#pragma once

#include <stdint.h>
#include <stddef.h>
#include <utils.h>

#define MEM_USABLE 1
#define MEM_RESERVED 2
#define MEM_ACPI_RECLAIMABLE 3
#define MEM_ACPI_NVS 4
#define MEM_BAD 5

struct mem_map_entry{
	uint64_t base;
	uint64_t length;
	uint32_t type;
	uint32_t acpi_extended;
};
typedef struct mem_map_entry mem_map_entry_t;

void init_kmalloc();

void* kmalloc(uint64_t size,uint64_t alignment);
void kfree(void* base, uint64_t size);

