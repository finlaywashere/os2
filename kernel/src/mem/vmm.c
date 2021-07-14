#include <mem/vmm.h>

void* kmalloc_p(uint64_t size){
	void* pointer = kmalloc(size,1);
	uint64_t pointer_int = (uint64_t) pointer;
	pointer_int += 0xffffff8000000000;
	memset((uint8_t*) pointer_int, 0x0, size); // Initialize memory to 0's
	return (void*)pointer_int;
}
void* kmalloc_pa(uint64_t size, uint64_t align){
	void* pointer = kmalloc(size,align);
	uint64_t pointer_int = (uint64_t) pointer;
	pointer_int += 0xffffff8000000000;
	memset((uint8_t*) pointer_int, 0x0, size); // Initialize memory to 0's
	return (void*)pointer_int;
}

void kfree_p(void* base, uint64_t size){
	uint64_t pointer_int = (uint64_t) base;
	pointer_int -= 0xffffff8000000000;
	kfree((void*)pointer_int,size);
}
