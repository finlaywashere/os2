#include <arch/x86_64/gdt.h>

gdt_entry_t* gdt;

gdt_ptr_t gdt_ptr;

void init_gdt(){
	gdt = (gdt_entry_t*) kmalloc_p(sizeof(gdt_entry_t)*3);
	
	set_gdt_entry(0,0,0,0,0); // Null segment
	set_gdt_entry(1,0,0xFFFFFFFF,0b10011000, 0b10100000); // Kernel code segment
	set_gdt_entry(2,0,0xFFFFFFFF,0b10010010, 0b10000000); // Kernel data segment
	
	gdt_ptr.base = (uint64_t) gdt;
        gdt_ptr.limit = sizeof(gdt_entry_t)*3-1;
        load_gdt();
}
void set_gdt_entry(int index, uint32_t base, uint32_t limit, uint8_t access, uint8_t flags){
	gdt[index].limit_low = (uint16_t) limit;
	gdt[index].base_low = (uint16_t) base;
	gdt[index].base_mid = (uint8_t) (base >> 16);
	gdt[index].access = access;
	gdt[index].flags = flags | ((limit >> 16) & 0xF);
	gdt[index].base_high = (uint8_t) (base >> 24);
}
