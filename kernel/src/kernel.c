#include <kernel.h>

void kernel_start(){
	// Initialize all of the architecture specific components
	init_arch();
	log_debug("Initialized x86_64 drivers!\n");
	// Initialize all filesystems and setup the root filesystem
	init_filesystems();
	log_debug("Initialized filesystems!\n");
	// Initialize the syscall handler
	init_syscalls();
	log_debug("Initialized syscalls!\n");
	// Initialize the CPU scheduler
	init_processes();
	log_debug("Initialized processes!\n");
	// Allocate memory for a page table for the init process
	page_table_t* dst = (page_table_t*) kmalloc_p(sizeof(page_table_t));
	// Load test_c.o from disk
	uint64_t entry_point = load_elf("/shell.o", dst);
	// Create a process from the loaded program
	create_process(dst, entry_point,0,0,0,0,0);
	while(1){
		// Infinite loop
	}
}
