#include <mem/pmm.h>

const uint64_t KERNEL_VIRT_ADDR = 0xFFFFFFFF80000000;

extern uint64_t _kernel_end;

uint64_t kernel_phys_end;
uint64_t safe_alloc_start;
uint64_t safe_alloc_kib;
uint8_t* kmalloc_bitmap;
uint64_t bitmap_count;

void init_kmalloc(uint64_t memory_size){
	uint64_t* p4_table = (uint64_t*)0x8000;
	uint64_t* p3_table = (uint64_t*) (p4_table[511] & 0xFFFFFFFFFFFFF000); // Find physical location of kernel in memory before kernel paging code is initialized
	for(uint64_t i = 0; i < 16; i++){ // Map 16GiB to 0xffff800000000000
		uint64_t entry = (0x40000000 * i) | 0b10000011; // Huge, read, write
		p3_table[i] = entry;
	}
	uint64_t* p2_table = (uint64_t*) (p3_table[510] & 0xFFFFFFFFFFFFF000);
	uint64_t kphys_base = p2_table[0] & 0xFFFFFFFFFFFFF000;
	uint64_t offset = KERNEL_VIRT_ADDR-kphys_base;
	kernel_phys_end = _kernel_end - offset;
	safe_alloc_start = kernel_phys_end + 0x800000; // Add 8MiB buffer after kernel for stacks/general safety
	bitmap_count = memory_size/8;
	kmalloc_bitmap = (uint8_t*)safe_alloc_start;
	safe_alloc_start += 2 * bitmap_count; // Add bitmap to the end of that safe allocation start, and double its size for extra safety
	safe_alloc_kib = safe_alloc_start/1024+1; // Convert bytes to KiB and add 1 because I don't want to check for remainders
	for(uint64_t i = 0; i < bitmap_count; i++){
		uint8_t entry = 0;
		for(uint64_t j = 0; j < 8; j++){
			uint8_t value = 1;
			if(i <= safe_alloc_kib)
				value = 0;
			entry |= value << j;
		}
		kmalloc_bitmap[i] = entry;
	}
}

void* kmalloc(uint64_t size){
	for(uint64_t i = safe_alloc_kib/8; i < bitmap_count; i++){ // Start looking after the safe allocation point to save cpu cycles
		uint8_t found = 1;
		uint64_t j;
		for(j = 0; j < size; j++){
			uint64_t index = (i + j) / 8;
			uint64_t bit = (i + j) % 8;
			uint8_t value = kmalloc_bitmap[index] & (1 << bit);
			if(value == 0){
				found = 0;
				break;
			}
		}
		if(found == 1){
			for(j = 0; j < size; j++){
				uint64_t index = (i + j) / 8;
                        	uint64_t bit = (i + j) % 8;
				uint8_t entry = kmalloc_bitmap[index] & !(1 << bit);
				kmalloc_bitmap[index] = entry;
			}
			return (void*)i;
		}else{
			i += j/8; // Increase by how many we looked at so we know to skip over those
			continue;
		}
	}
	return (void*) 0x0;
}
void kfree(void* base, uint64_t size){
	//TODO: Implement kfree!
}
