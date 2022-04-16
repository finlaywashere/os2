#ifndef PROCESS_H
#define PROCESS_H

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/tty.h>
#include <mem/vmm.h>
#include <utils.h>
#include <log.h>
#include <fs/fs.h>
#include <arch/x86_64/driver/keyboard.h>

#define FLAG_KERNEL 1

#define PROCESS_DEAD 0
#define PROCESS_RUNNING 1
#define PROCESS_SLEEPING 2
#define PROCESS_READY 3
#define PROCESS_WAIT 4

#define WAIT_PID 1

#define DESCRIPTOR_PRESENT 1

#define MAX_PROCESS_COUNT 20
#define MAX_DESCRIPTOR_COUNT 20
#define MAX_FILE_COUNT 50
#define STACK_SIZE 16384
#define BUFFER_SIZE 512
#define MAX_SIGNALS 128

#define MODE_WRITE 1

#define HANDLER_ERROR 1
#define HANDLER_TIMER 2


struct scheduler_info{
	uint8_t priority;
	uint8_t wait_type;
	uint64_t wait_condition;
};
typedef struct scheduler_info scheduler_info_t;

struct signal_handler{
	uint64_t handler;
	uint64_t flags;
	uint64_t condition;
};
typedef struct signal_handler signal_handler_t;

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
	fs_node_t* root_directory;
	fs_node_t* current_directory;
	uint64_t uid;
	uint64_t gid;
	uint64_t flags;
	uint64_t wait_condition;
	descriptor_t descriptors[MAX_DESCRIPTOR_COUNT];
	fs_node_t files[MAX_FILE_COUNT];
	scheduler_info_t sch_info;
	signal_handler_t signal_handlers[MAX_SIGNALS];
};

typedef struct process process_t;

void init_processes();
void schedule(registers_t* regs);
uint64_t create_process(page_table_t* loaded_data, uint64_t entry, uint64_t parent, char* argv[], int argc, char* envp[], int envc, uint8_t interrupts);
void create_process_pid(uint64_t pid, page_table_t* loaded_data, uint64_t entry, uint64_t parent, char* argv[], int argc, char* envp[], int envc, uint8_t interrupts);
void create_process_pid_nodesc(uint64_t pid, page_table_t* loaded_data, uint64_t entry, char* argv[], int argc, char* envp[], int envc, uint8_t interrupts);
uint64_t create_file_descriptor(uint64_t pid, char* filename, uint64_t size);
void kill_process(uint64_t code);
process_t* get_process();
uint8_t configure_descriptors(uint64_t pid, uint64_t parent);
uint64_t fork_process(uint64_t parent, registers_t* regs);
uint64_t get_curr_process();
uint64_t open_file_descriptor(char* name, uint64_t mode);
descriptor_t* get_descriptor(uint64_t id);
fs_node_t* get_descriptor_file(uint64_t id);
void close_file_descriptor(uint64_t id);
void process_wait(uint64_t pid, registers_t* regs);
int process_error(registers_t *regs);
int is_kernel();
#endif
