#include <arch/x86_64/idt.h>

idt_t* idt;

idt_ptr_t idt_ptr;

void init_idt(){
	idt = kmalloc_p(sizeof(idt)*256);
	// Remap the PIC
	outb(0x20,0x11);
	outb(0xA0, 0x11);
	outb(0x21, 0x20);
	outb(0xA1, 40);
	outb(0x21, 0x04);
	outb(0xA1, 0x02);
	outb(0x21, 0x01);
	outb(0xA1, 0x01);
	outb(0x21, 0x0);
	outb(0xA1, 0x0);
	
	set_idt_gate(32, (uint64_t) &irq0, 0x08, 0x8e);
	set_idt_gate(33, (uint64_t) &irq1, 0x08, 0x8e);
	set_idt_gate(34, (uint64_t) &irq2, 0x08, 0x8e);
	set_idt_gate(35, (uint64_t) &irq3, 0x08, 0x8e);
	set_idt_gate(36, (uint64_t) &irq4, 0x08, 0x8e);
	set_idt_gate(37, (uint64_t) &irq5, 0x08, 0x8e);
	set_idt_gate(38, (uint64_t) &irq6, 0x08, 0x8e);
	set_idt_gate(39, (uint64_t) &irq7, 0x08, 0x8e);
	set_idt_gate(40, (uint64_t) &irq8, 0x08, 0x8e);
	set_idt_gate(41, (uint64_t) &irq9, 0x08, 0x8e);
	set_idt_gate(42, (uint64_t) &irq10, 0x08, 0x8e);
	set_idt_gate(43, (uint64_t) &irq11, 0x08, 0x8e);
	set_idt_gate(44, (uint64_t) &irq12, 0x08, 0x8e);
	set_idt_gate(45, (uint64_t) &irq13, 0x08, 0x8e);
	set_idt_gate(46, (uint64_t) &irq14, 0x08, 0x8e);
	set_idt_gate(47, (uint64_t) &irq15, 0x08, 0x8e);
	
	// The base is linear
	idt_ptr.base = (uint64_t) idt;//get_physical_addr((uint64_t) idt);
	idt_ptr.limit = sizeof(idt_t)*256;
	load_idt();
}
void set_idt_gate(int index, uint64_t addr, uint16_t selector, uint8_t type){
	idt[index].offset_low = (uint16_t) addr;
	idt[index].offset_mid = (uint16_t) (addr >> 16);
	idt[index].offset_high = (uint32_t) (addr >> 32);
	idt[index].reserved1 = 0;
	idt[index].reserved2 = 0;
	idt[index].type = type;
	idt[index].selector = selector;
}
void irq_handler(registers_t regs){
	if(regs.interrupt >= 40){
		// Send more EOI's
		outb(0xA0,0x20);
	}
	outb(0x20,0x20); // EOI
	log_error("Received IRQ!");
}
