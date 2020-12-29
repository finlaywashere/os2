#pragma once

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
	uint64_t gs, fs, es, ss, ds;
	uint64_t cr2;
	uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
	uint64_t rbp, rsp, rdi, rdx, rcx, rbx, rax;
	uint64_t interrupt, error;
	uint64_t rip;
	uint64_t cs;
	uint64_t eflags, userrsp;
	uint64_t userss;
}__attribute__((packed));

typedef struct registers registers_t;

extern void load_idt();
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

void init_idt();
void set_idt_gate(int index, uint64_t addr, uint16_t selector, uint8_t type);
void irq_handler(registers_t regs);
