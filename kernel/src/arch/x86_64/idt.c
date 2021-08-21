#include <arch/x86_64/idt.h>
#include <process.h>

idt_t* idt;

idt_ptr_t idt_ptr;

isr_t* interrupt_handlers;

void init_idt(){
	idt = (idt_t*) kmalloc_p(sizeof(idt)*256);
	interrupt_handlers = (isr_t*) kmalloc_p(sizeof(isr_t)*256);
	memset((uint8_t*)interrupt_handlers,0,sizeof(isr_t)*256);
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
	
	set_idt_gate(0, (uint64_t) &isr0, 0x08, 0x8e);
	set_idt_gate(1, (uint64_t) &isr1, 0x08, 0x8e);
	set_idt_gate(2, (uint64_t) &isr2, 0x08, 0x8e);
	set_idt_gate(3, (uint64_t) &isr3, 0x08, 0x8e);
	set_idt_gate(4, (uint64_t) &isr4, 0x08, 0x8e);
	set_idt_gate(5, (uint64_t) &isr5, 0x08, 0x8e);
	set_idt_gate(6, (uint64_t) &isr6, 0x08, 0x8e);
	set_idt_gate(7, (uint64_t) &isr7, 0x08, 0x8e);
	set_idt_gate(8, (uint64_t) &isr8, 0x08, 0x8e);
	set_idt_gate(9, (uint64_t) &isr9, 0x08, 0x8e);
	set_idt_gate(10, (uint64_t) &isr10, 0x08, 0x8e);
	set_idt_gate(11, (uint64_t) &isr11, 0x08, 0x8e);
	set_idt_gate(12, (uint64_t) &isr12, 0x08, 0x8e);
	set_idt_gate(13, (uint64_t) &isr13, 0x08, 0x8e);
	set_idt_gate(14, (uint64_t) &isr14, 0x08, 0x8e);
	set_idt_gate(15, (uint64_t) &isr15, 0x08, 0x8e);
	set_idt_gate(16, (uint64_t) &isr16, 0x08, 0x8e);
	set_idt_gate(17, (uint64_t) &isr17, 0x08, 0x8e);
	set_idt_gate(18, (uint64_t) &isr18, 0x08, 0x8e);
	set_idt_gate(19, (uint64_t) &isr19, 0x08, 0x8e);
	set_idt_gate(20, (uint64_t) &isr20, 0x08, 0x8e);
	set_idt_gate(21, (uint64_t) &isr21, 0x08, 0x8e);
	set_idt_gate(22, (uint64_t) &isr22, 0x08, 0x8e);
	set_idt_gate(23, (uint64_t) &isr23, 0x08, 0x8e);
	set_idt_gate(24, (uint64_t) &isr24, 0x08, 0x8e);
	set_idt_gate(25, (uint64_t) &isr25, 0x08, 0x8e);
	set_idt_gate(26, (uint64_t) &isr26, 0x08, 0x8e);
	set_idt_gate(27, (uint64_t) &isr27, 0x08, 0x8e);
	set_idt_gate(28, (uint64_t) &isr28, 0x08, 0x8e);
	set_idt_gate(29, (uint64_t) &isr29, 0x08, 0x8e);
	set_idt_gate(30, (uint64_t) &isr30, 0x08, 0x8e);
	set_idt_gate(31, (uint64_t) &isr31, 0x08, 0x8e);
	
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

	set_idt_gate(80, (uint64_t) &irq80, 0x08, 0xee); // Syscall interrupt, can be triggered from ring 3
	
	// The base is linear
	idt_ptr.base = (uint64_t) idt;//get_physical_addr((uint64_t) idt);
	idt_ptr.limit = sizeof(idt_t)*256-1;
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
void isr_handler(registers_t regs){
	if(regs.interrupt >= 40){
		// Send more EOI's
		outb(0xA0,0x20);
	}
	outb(0x20,0x20); // EOI
	if(regs.interrupt < 32){
		log_error("\nReceived error 0x");
		log_error_num(regs.interrupt, 16);
		log_error("!\nProcess: ");
		log_error_num(get_curr_process(),10);
		log_error("\nPC: 0x");
		log_error_num(regs.rip,16);
		log_error("\nError code: 0x");
		log_error_num(regs.error,16);
		if(regs.interrupt == 0xE){ // Page fault
			log_error("\nAccess Address: 0x");
			log_error_num(regs.cr2,16);
		}
		disable_interrupts();
		panic("Error Occurred");
	}
	if(regs.interrupt >= 32 && interrupt_handlers[regs.interrupt] != 0){
		isr_t handler = interrupt_handlers[regs.interrupt];
		handler(&regs);
	}
}
void isr_register_handler(uint8_t num, isr_t handler){
	interrupt_handlers[num] = handler;
}
