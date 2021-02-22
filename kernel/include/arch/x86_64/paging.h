#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>
#include <stddef.h>
#include <mem/vmm.h>

struct page_table{
	uint64_t entries[512];
}__attribute__((packed));
typedef struct page_table page_table_t;

void init_paging();
page_table_t* get_page_directory();
void flush_tlb(uint64_t addr);
uint64_t get_physical_addr(uint64_t virtual);
void map_page(uint64_t phys, uint64_t virtual, uint8_t flags);
void map_pages(uint64_t phys, uint64_t virtual, uint8_t flags, uint64_t size);
void set_page_directory(page_table_t* pd);
page_table_t* get_curr_page_directory();
page_table_t* hard_copy(page_table_t* table);
uint64_t find_first_available_page();
#endif
