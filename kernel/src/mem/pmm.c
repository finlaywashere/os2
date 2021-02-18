#include <mem/pmm.h>

const uint64_t KERNEL_VIRT_ADDR = 0xFFFFFFFF80000000;

extern uint64_t _kernel_end; // Note: this is actually a pointer so its value is &_kernel_end!

uint64_t kernel_phys_end;
uint64_t safe_alloc_start;
uint64_t safe_alloc_kib;
uint8_t* kmalloc_bitmap;
uint64_t bitmap_count;

void init_kmalloc(){
	uint32_t mem_map_length = *(uint32_t*) 0xffffff8000006000;
	uint64_t mem_map_addr = 0xffffff8000006020;
	mem_map_entry_t* mem_map = (mem_map_entry_t*) mem_map_addr;
	
	uint64_t mem_size = 0;
	
	for(int i = 0; i < mem_map_length; i++){
		mem_map_entry_t entry = mem_map[i];
		if(entry.base+entry.length > mem_size)
			mem_size = entry.base+entry.length;
	}
	
	
	kernel_phys_end = ((uint64_t) &_kernel_end) - KERNEL_VIRT_ADDR;
	safe_alloc_start = kernel_phys_end + 0x800000; // Add 8MiB buffer after kernel for stacks/general safety
	bitmap_count = mem_size/8192;
	kmalloc_bitmap = (uint8_t*)(safe_alloc_start+0xffffff8000000000);
	memset(kmalloc_bitmap,0,bitmap_count);
	safe_alloc_start += 2 * bitmap_count; // Add bitmap to the end of that safe allocation start, and double its size for extra safety
	safe_alloc_kib = safe_alloc_start/1024+1; // Convert bytes to KiB and add 1 because I don't want to check for remainders
	for(int i = 0; i < mem_map_length; i++){
		mem_map_entry_t entry = mem_map[i];
		if(entry.type != MEM_USABLE)
			continue;
		for(uint64_t addr = entry.base/8192+1; addr < (entry.base+entry.length)/8192-1; addr++){
			uint8_t bitmap_entry = 0;
			for(uint64_t j = 0; j < 8; j++){
				uint8_t value = 0;
        	                if(addr*8+j > safe_alloc_kib)
                	                value = 1;
	                	bitmap_entry |= value << j;
			}
			kmalloc_bitmap[addr] = bitmap_entry;
		}
	}
}

void* kmalloc(uint64_t size, uint64_t alignment){
	// This is a very dumb way to do it, once 1 part of a 8KiB page represented by one of these bytes is set then the whole page is effectively allocated
	// I'm going to leave making this better as a future todo
	// TODO: Improve this
	size = size/1024 + ((size % 1024) > 0 ? 1 : 0); // 1KiB pages
	for(uint64_t i = safe_alloc_kib/8; i < bitmap_count; i++){ // Start looking after the safe allocation point to save cpu cycles
		if((i*1024*8) % alignment > 0){
			continue;
		}
		uint8_t found = 1;
		uint64_t j;
		for(j = 0; j < size; j++){
			uint64_t index = i + (j / 8);
			uint64_t bit = (i + j) % 8;
			uint8_t value = kmalloc_bitmap[index] & (1 << bit);
			if(value == 0){
				found = 0;
				break;
			}
		}
		if(found == 1){
			for(j = 0; j < size; j++){
				uint64_t index = i + (j / 8);
                        	uint64_t bit = (i + j) % 8;
				uint8_t entry = kmalloc_bitmap[index] & !(1 << bit);
				kmalloc_bitmap[index] = entry;
			}
			return (void*)(i*1024*8);
		}
	}
	return (void*) 0x0;
}
void kfree(void* base, uint64_t size){
	uint64_t addr = (uint64_t) base;
	size = size/1024 + ((size % 1024) > 0 ? 1 : 0); // 1KiB pages
	uint64_t i = addr/(1024*8);
	uint64_t end = i + size;
	for(; i < end; i++){
		uint64_t page_index = i / 8;
		uint64_t page_bit = i % 8;
		uint8_t entry = kmalloc_bitmap[page_index] & !(1 << page_bit);
		kmalloc_bitmap[page_index] = entry;
	}
}
