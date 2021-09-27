#include <arch/x86_64/paging.h>

page_table_t* page_directory;

void init_paging(){
	page_directory = get_page_directory();
	page_table_t* p3_table = (page_table_t*) (page_directory->entries[511] & 0xFFFFFFFFFFFFF000);
        for(uint64_t i = 0; i < 16; i++){ // Map 16GiB to 0xffffff8000000000
                uint64_t entry = (0x40000000 * i) | 0b10000011; // Huge, read, write
                p3_table->entries[i] = entry;
        }
	page_directory = (page_table_t*) (((uint64_t)page_directory)+0xffffff8000000000);
	page_directory->entries[0] = 0x0; // Clear identity mappings
}
uint64_t phys_to_virt(uint64_t physical){
	return physical + 0xffffff8000000000;
}

void set_page_directory(page_table_t* pd){
	asm volatile("mov %0, %%cr3" ::"r" (get_physical_addr((uint64_t) pd)));
	page_directory = pd;
}

page_table_t* get_curr_page_directory(){
	return page_directory;
}

page_table_t* get_page_directory(){
	uint64_t cr3;
	asm ("mov %%cr3, %0": "=r"(cr3));
	return (page_table_t*) cr3;
}
void flush_tlb(uint64_t addr){
	asm volatile("invlpg (%0)" :: "r" (addr) : "memory");
}
uint64_t find_first_available_page(){
	for(int i1 = 0; i1 < 512; i1++){
        if(i1 > 128){
        	return -1;
		}
        uint64_t p4_entry = page_directory->entries[i1];
        if(p4_entry == 0){
            return i1 << 39;
		}
        page_table_t* p3_table = (page_table_t*) ((p4_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000); // Get P3 table in memory
        for(int i2 = 0; i2 < 512; i2++){
            uint64_t p3_entry = p3_table->entries[i2];
            if(p3_entry == 0)
                return (i1 << 39) | (i2 << 30);
            page_table_t* p2_table = (page_table_t*) ((p3_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000); // Get P2 table in memory
            for(int i3 = 0; i3 < 512; i3++){
                uint64_t p2_entry = p2_table->entries[i3];
                if(p2_entry == 0)
                    return (i1 << 39) | (i2 << 30) | (i3 << 21);
            }
        }
    }
	return -1;
}
uint64_t get_physical_addr(uint64_t virtual){
	int p4_index = (virtual >> 39) & 0x1FF; // Get index in P4 table
	uint64_t p4_entry = page_directory->entries[p4_index];
	page_table_t* p3_table = (page_table_t*)((uint64_t)(p4_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000);
	int p3_index = (virtual >> 30) & 0x1FF; // Get index in P3 table
	uint64_t p3_entry = p3_table->entries[p3_index];
	if(p3_entry & (1 << 7)){ // Is it a 1G page (huge bit is set in P3)
		uint64_t phys = p3_entry & 0xFFFFFFFFFFFFF000; // Physical location p3 entry represents
		phys += virtual % 0x40000000; // Add remainder after phys
		return phys;
	}else{
		page_table_t* p2_table = (page_table_t*) ((uint64_t)(p3_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000); // Get P2 table in memory
		int p2_index = (virtual >> 21) & 0x1FF; // Get index in P2 table
		uint64_t p2_entry = p2_table->entries[p2_index];
		if(p2_entry & (1 << 7)){ // Is it a 2MiB page (huge bit is set in P2)
			uint64_t phys = p2_entry & 0xFFFFFFFFFFFFF000; // Physical location p3 entry represents
			phys += virtual % 0x200000; // Add remainder after phys
			return phys;
		}else{
			page_table_t* p1_table = (page_table_t*) ((uint64_t)(p2_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000);
			int p1_index = (virtual >> 12) & 0x1FF; // Get index in P1 table
			uint64_t p1_entry = p1_table->entries[p1_index];
			uint64_t phys = p1_entry & 0xFFFFFFFFFFFFF000;
			phys += virtual % 0x1000; // Add remainder after phys
			return phys;
		}
	}
}
void map_pages(uint64_t phys, uint64_t virtual, uint8_t flags, uint64_t size){
	uint64_t count = size / 0x200000 + ((size % 0x200000) > 0 ? 1 : 0);
	for(uint64_t i = 0; i < count; i++){
		map_page(phys,virtual,flags);
		phys += 0x200000;
		virtual += 0x200000;
	}
}
uint64_t alloc_table(){
	return (uint64_t) kmalloc_p(4096);
}
page_table_t* hard_copy(page_table_t* table){
	page_table_t* dst = (page_table_t*) alloc_table();
	for(int i1 = 0; i1 < 512; i1++){
		if(i1 > 128){
			dst->entries[i1] = table->entries[i1];
			continue;
		}
		uint64_t p4_entry = table->entries[i1];
		if(p4_entry == 0)
			continue;
		page_table_t* p3_table = (page_table_t*) ((p4_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000); // Get P3 table in memory
		page_table_t* new_p3_table = (page_table_t*) alloc_table();
		dst->entries[i1] = (((uint64_t) new_p3_table) - 0xffffff8000000000) | (p4_entry & 0xFFF); // Copy flags but change address
		for(int i2 = 0; i2 < 512; i2++){
			uint64_t p3_entry = p3_table->entries[i2];
			if(p3_entry == 0)
				continue;
			page_table_t* p2_table = (page_table_t*) ((p3_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000); // Get P2 table in memory
			page_table_t* new_p2_table = (page_table_t*) alloc_table();
			new_p3_table->entries[i2] = (((uint64_t) new_p2_table) - 0xffffff8000000000) | (p3_entry & 0xFFF); // Copy flags but change address
			for(int i3 = 0; i3 < 512; i3++){
				uint64_t p2_entry = p2_table->entries[i3];
				if(p2_entry == 0)
					continue;
				uint8_t* old_entry = ((p2_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000);
				uint8_t* new_entry = (uint8_t*) kmalloc_p(0x200000); // Allocate a page of memory
				memcpy(old_entry,new_entry,0x200000);
				new_p2_table->entries[i3] = (((uint64_t) new_entry) - 0xffffff8000000000) | (p2_entry & 0xFFF); // Copy flags but change address
			}
		}
	}
	return dst;
}
void map_page(uint64_t phys, uint64_t virtual, uint8_t flags){
	flags |= 0b10000000;
	int p4_index = (virtual >> 39) & 0x1FF; // Get index in P4 table
        uint64_t p4_entry = page_directory->entries[p4_index];
        page_table_t* p3_table = (page_table_t*)((p4_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000);
	if(p3_table == 0xffffff8000000000){
		p3_table = (page_table_t*) alloc_table();
		page_directory->entries[p4_index] = (((uint64_t)p3_table)-0xffffff8000000000) | 0x7; // Present, RW, User
	}
        int p3_index = (virtual >> 30) & 0x1FF; // Get index in P3 table
        uint64_t p3_entry = p3_table->entries[p3_index];
	page_table_t* p2_table = (page_table_t*) ((p3_entry & 0xFFFFFFFFFFFFF000) + 0xffffff8000000000); // Get P2 table in memory
	if(p2_table == 0xffffff8000000000){
                p2_table = (page_table_t*) alloc_table();
                p3_table->entries[p3_index] = (((uint64_t)p2_table)-0xffffff8000000000) | 0x7; // Present, RW, User
        }
        int p2_index = (virtual >> 21) & 0x1FF; // Get index in P2 table
	
	uint64_t entry = (phys & 0xFFFFFFFFFFFFF000) | flags;
	p2_table->entries[p2_index] = entry;
	flush_tlb(virtual);
}
