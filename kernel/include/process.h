#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/tty.h>
#include <mem/vmm.h>
#include <utils.h>

#define PROCESS_DEAD 0
#define PROCESS_RUNNING 1
#define PROCESS_SLEEPING 2
#define PROCESS_READY 3

#define DESCRIPTOR_PRESENT 1

#define MAX_PROCESS_COUNT 20
#define MAX_DESCRIPTOR_COUNT 20
#define STACK_SIZE 16384
#define BUFFER_SIZE 512

struct descriptor{
	uint8_t flags;
	uint64_t id;
	uint8_t* buffer;
	uint64_t buffer_size;
	uint64_t buffer_seek;
	uint64_t (*read) (struct descriptor *descriptor, uint8_t* buffer, uint64_t size);
	uint64_t (*write) (struct descriptor *descriptor, uint8_t* buffer, uint64_t size);
	void (*seek) (struct descriptor *descriptor, uint64_t pos);
};
typedef struct descriptor descriptor_t;

struct process{
	page_table_t* page_table;
	registers_t regs;
	uint64_t shadow_stack;
	uint16_t status;
	uint64_t parent;
	uint64_t count;
	descriptor_t descriptors[MAX_DESCRIPTOR_COUNT];
};

typedef struct process process_t;

void init_processes();
void schedule(registers_t* regs);
uint64_t create_process(page_table_t* loaded_data, uint64_t entry, uint64_t parent);
void create_process_pid(uint64_t pid, page_table_t* loaded_data, uint64_t entry, uint64_t parent);
void create_process_pid_nodesc(uint64_t pid, page_table_t* loaded_data, uint64_t entry);
uint64_t create_file_descriptor(uint64_t pid, char* filename, uint64_t size);
void kill_process(uint64_t code);
process_t* get_process();
uint8_t configure_descriptors(uint64_t pid, uint64_t parent);
uint64_t fork_process(uint64_t parent, registers_t* regs);
uint64_t get_curr_process();

#endif
