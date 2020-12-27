#include <stdint.h>
#include <stddef.h>
#include <mem/pmm.h>

void* kmalloc_p(uint64_t size);
void kfree_p(void* base, uint64_t size);

