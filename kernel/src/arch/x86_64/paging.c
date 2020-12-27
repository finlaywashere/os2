#include <arch/x86_64/paging.h>

uint64_t* page_directory;

void init_paging(){
	page_directory = get_page_directory();
	uint64_t* p3_table = (uint64_t*) (page_directory[511] & 0xFFFFFFFFFFFFF000);
        for(uint64_t i = 0; i < 16; i++){ // Map 16GiB to 0xffffff8000000000
                uint64_t entry = (0x40000000 * i) | 0b10000011; // Huge, read, write
                p3_table[i] = entry;
        }
	page_directory = (uint64_t*) (((uint64_t)page_directory)+0xffffff8000000000);
	page_directory[0] = 0x0; // Clear identity mappings
}

uint64_t* get_page_directory(){
	uint64_t cr3;
	asm ("mov %%cr3, %0": "=r"(cr3));
	return (uint64_t*) cr3;
}
void flush_tlb(uint64_t addr){
	asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}
uint64_t get_physical_addr(uint64_t virtual){
	int p4_index = (virtual >> 39) & 0x1FF; // Get index in P4 table
	uint64_t p4_entry = page_directory[p4_index];
	uint64_t* p3_table = (uint64_t*)(p4_entry & 0xFFFFFFFFFFFFF000);
	int p3_index = (virtual >> 30) & 0x1FF; // Get index in P3 table
	uint64_t p3_entry = p3_table[p3_index];
	if(p3_entry & (1 << 7)){ // Is it a 1G page (huge bit is set in P3)
		uint64_t phys = p3_entry & 0xFFFFFFFFFFFFF000; // Physical location p3 entry represents
		phys += virtual % 0x40000000; // Add remainder after phys
		return phys;
	}else{
		uint64_t* p2_table = (uint64_t*) (p3_entry & 0xFFFFFFFFFFFFF000); // Get P2 table in memory
		int p2_index = (virtual >> 21) & 0x1FF; // Get index in P2 table
		uint64_t p2_entry = p2_table[p2_index];
		if(p2_entry & (1 << 7)){ // Is it a 2MiB page (huge bit is set in P2)
			uint64_t phys = p2_entry & 0xFFFFFFFFFFFFF000; // Physical location p3 entry represents
			phys += virtual % 0x200000; // Add remainder after phys
			return phys;
		}else{
			uint64_t* p1_table = (uint64_t*) (p2_entry & 0xFFFFFFFFFFFFF000);
			int p1_index = (virtual >> 12) & 0x1FF; // Get index in P1 table
			uint64_t p1_entry = p1_table[p1_index];
			uint64_t phys = p1_entry & 0xFFFFFFFFFFFFF000;
			phys += virtual % 0x1000; // Add remainder after phys
			return phys;
		}
	}
}
