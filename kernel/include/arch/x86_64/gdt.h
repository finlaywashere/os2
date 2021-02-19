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

struct tss{
	uint32_t reserved1;
	uint64_t rsp0;
	uint64_t rsp1;
	uint64_t rsp2;
	uint64_t reserved2;
	uint64_t ist1;
	uint64_t ist2;
	uint64_t ist3;
	uint64_t ist4;
	uint64_t ist5;
	uint64_t ist6;
	uint64_t ist7;
	uint64_t reserved3;
	uint16_t reserved4;
	uint16_t iopb_offset;
}__attribute__((packed));

typedef struct tss tss_t;

extern void load_gdt();

void init_gdt();
void set_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags);
void tss_set_rsp(uint64_t rsp);
