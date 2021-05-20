#include <kernel.h>
#include <arch/x86_64/cpuid.h>

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
	int result = check_cpuid();
	if(result == 0)
		log_error("CPUID not supported!");
	cpuid_t *c_dst = (cpuid_t*) kmalloc_p(sizeof(cpuid_t));
	cpuid(0,c_dst);
	uint32_t vendor[3];
	vendor[0] = c_dst->ebx;
	vendor[1] = c_dst->edx;
	vendor[2] = c_dst->ecx;
	log_debugl((char*) vendor,12);

	// Allocate memory for a page table for the init process
	page_table_t* dst = (page_table_t*) kmalloc_p(sizeof(page_table_t));
	// Load test_c.o from disk
	uint64_t entry_point = load_elf("/shell.o", dst);
	// Create a process from the loaded program
	create_process(dst, entry_point,0,0,0,0,0,1);
	while(1){
		// Infinite loop
	}
}
