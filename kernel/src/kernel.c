#include <kernel.h>

void kernel_start(){
	init_arch();
	log_debug("Initialized x86_64 drivers!\n");
	init_filesystems();
	log_debug("Initialized filesystems!\n");
	init_syscalls();
	log_debug("Initialized syscalls!\n");
	init_processes();
	log_debug("Initialized processes!\n");
	page_table_t* dst = (page_table_t*) kmalloc_p(sizeof(page_table_t));
	uint64_t entry_point = load_elf("/test_c.o", dst);
	create_process(dst, entry_point,0,0,0,0,0);
	while(1){
		// Infinite loop
	}
}
