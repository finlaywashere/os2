#pragma once

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/idt.h>
#include <mem/vmm.h>
#include <utils.h>

#define PROCESS_DEAD 0
#define PROCESS_RUNNING 1
#define PROCESS_SLEEPING 2
#define PROCESS_READY 3

#define MAX_PROCESS_COUNT 20
#define STACK_SIZE 16384

struct process{
	page_table_t* page_table;
	registers_t regs;
	uint64_t shadow_stack;
	uint16_t status;
};

typedef struct process process_t;

void init_processes();
void schedule(registers_t* regs);
uint64_t create_process(page_table_t* loaded_data, uint64_t entry);
void kill_process(uint64_t code);
