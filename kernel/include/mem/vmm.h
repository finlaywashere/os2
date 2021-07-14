#include <stdint.h>
#include <stddef.h>
#include <mem/pmm.h>
#include <utils.h>

void* kmalloc_p(uint64_t size);
void* kmalloc_pa(uint64_t size, uint64_t align);
void kfree_p(void* base, uint64_t size);

