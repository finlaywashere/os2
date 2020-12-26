#include <stdint.h>
#include <stddef.h>

void init_kmalloc(uint64_t memory_size); // Note: memory size is in KiB

void* kmalloc(uint64_t size);
void kfree(void* base, uint64_t size);

