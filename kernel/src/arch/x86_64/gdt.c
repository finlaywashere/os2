#include <arch/x86_64/gdt.h>

gdt_entry_t* gdt;
tss_t* tss;

gdt_ptr_t gdt_ptr;

void tss_set_rsp(uint64_t rsp){
	tss->rsp0 = rsp;
}

void init_gdt(){
	gdt = (gdt_entry_t*) kmalloc_p(sizeof(gdt_entry_t)*7);
	tss = (tss_t*) kmalloc_p(sizeof(tss_t));
	
	set_gdt_entry(0,0,0,0,0); // Null segment
	set_gdt_entry(1,0,0xFFFFFFFF,0b10011000, 0b10100000); // Kernel code segment
	set_gdt_entry(2,0,0xFFFFFFFF,0b10010010, 0b10000000); // Kernel data segment
	set_gdt_entry(3,0,0xFFFFFFFF,0b11111000, 0b10100000); // User code segment
	set_gdt_entry(4,0,0xFFFFFFFF,0b11110010, 0b10000000); // User data segment
	
	uint64_t tss_ptr = (uint64_t) tss;
	
	set_gdt_entry(5,(uint32_t) tss_ptr,104,0x89,0x40); // TSS low stuff
	set_gdt_entry(6,(uint32_t) ((uint16_t) (tss_ptr >> 48)),(uint32_t)((uint16_t) (tss_ptr >> 32)),0,0); // TSS high stuff
	
	gdt_ptr.base = (uint64_t) gdt;
        gdt_ptr.limit = sizeof(gdt_entry_t)*7-1;
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
