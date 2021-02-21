#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include <utils.h>
#include <mem/vmm.h>
#include <arch/x86_64/paging.h>
#include <log.h>

struct idt{
	uint16_t offset_low;
	uint16_t selector;
	uint8_t reserved1;
	uint8_t type;
	uint16_t offset_mid;
	uint32_t offset_high;
	uint32_t reserved2;
}__attribute__((packed));

typedef struct idt idt_t;

struct idt_ptr{
	uint16_t limit;
	uint64_t base;
}__attribute__((packed));

typedef struct idt_ptr idt_ptr_t;

struct registers{
	uint64_t gs, fs, es, ds;
	uint64_t cr2;
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rsi, rdi, rdx, rcx, rbx, rax;
	uint64_t interrupt, error;
	uint64_t rip;
	uint64_t cs;
	uint64_t rflags, userrsp;
	uint64_t userss;
}__attribute__((packed));

typedef struct registers registers_t;

typedef void (*isr_t)(registers_t*);

void isr_register_handler(uint8_t num, isr_t handler);

// Convenient ways to enable/disable interrupts for important code
extern void enable_interrupts();
extern void disable_interrupts();

extern void load_idt();

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

extern void irq80();

void init_idt();
void set_idt_gate(int index, uint64_t addr, uint16_t selector, uint8_t type);
void isr_handler(registers_t regs);

#endif
