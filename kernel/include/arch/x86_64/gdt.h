#include <stdint.h>
#include <stddef.h>

#include <mem/vmm.h>

struct gdt_entry{
	uint16_t limit_low;
	uint16_t base_low;
	uint8_t base_mid;
	uint8_t access;
	uint8_t flags;
	uint8_t base_high;
}__attribute__((packed));

typedef struct gdt_entry gdt_entry_t;

struct gdt_ptr{
	uint16_t limit;
	uint64_t base;
}__attribute__((packed));

typedef struct gdt_ptr gdt_ptr_t;

extern void load_gdt();

void init_gdt();
void set_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
