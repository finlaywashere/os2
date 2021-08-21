#include <mem/pmm.h>

const uint64_t KERNEL_VIRT_ADDR = 0xFFFFFFFF80000000;

extern uint64_t _kernel_end; // Note: this is actually a pointer so its value is &_kernel_end!

uint64_t kernel_phys_end;
uint64_t safe_alloc_start;
uint64_t safe_alloc;
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
	
	
	kernel_phys_end = ((uint64_t) &_kernel_end) - KERNEL_VIRT_ADDR + 0x200000; // Add 2MiB because that is the physical offset of where the kernel is located
	safe_alloc_start = kernel_phys_end + 0x800000; // Add 8MiB buffer after kernel for stacks/general safety
	bitmap_count = mem_size/0x1000;
	kmalloc_bitmap = (uint8_t*)(safe_alloc_start+0xffffff8000000000);
	memset(kmalloc_bitmap,0,bitmap_count);
	safe_alloc_start += 2 * bitmap_count; // Add bitmap to the end of that safe allocation start, and double its size for extra safety
	safe_alloc = safe_alloc_start/0x1000 + 1; // Convert bytes to 1 KiB chunks and add 1 because I don't want to check for remainders
	for(int i = 0; i < mem_map_length; i++){
		mem_map_entry_t entry = mem_map[i];
		if(entry.type != MEM_USABLE)
			continue;
		int max = (entry.base+entry.length)/0x1000;
		if(max > 0)
			max--;
		for(uint64_t addr = entry.base/0x1000+1; addr < max; addr++){
			if(addr > safe_alloc)
				kmalloc_bitmap[addr] = 1;
		}
	}
}

void* kmalloc(uint64_t size, uint64_t alignment){
	size = size/0x1000 + ((size % 0x1000) > 0 ? 1 : 0); // 1KiB pages
	for(uint64_t i = safe_alloc; i < bitmap_count; i++){ // Start looking after the safe allocation point to save cpu cycles
		if((i*0x1000) % alignment > 0){
			continue;
		}
		if(kmalloc_bitmap[i] == 0)
			continue;
		uint8_t found = 1;
		uint64_t j;
		for(j = 0; j < size; j++){
			uint64_t index = i + j;
			uint8_t value = kmalloc_bitmap[index];
			if(value == 0){
				found = 0;
				break;
			}
		}
		if(found == 1){
			for(j = 0; j < size; j++){
				uint64_t index = i + j;
				kmalloc_bitmap[index] = 0;
			}
			return (void*)(i*0x1000);
		}
	}
	panic("Error: OOM");
}
void kfree(void* base, uint64_t size){
	uint64_t addr = (uint64_t) base;
	size = size/0x1000 + ((size % 0x1000) > 0 ? 1 : 0); // 1KiB pages
	uint64_t i = addr/0x1000;
	uint64_t end = i + size;
	for(; i < end; i++){
		kmalloc_bitmap[i] = 1;
	}
}
