#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64/tty.h>
#include <mem/pmm.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/acpi.h>
#include <log.h>

void kernel_start(){
	init_paging();
	init_kmalloc(1024*100); // Assume 100MiB of memory
	init_ttys(1);
	log_debug("Initialized core kernel!\n");
	init_acpi();
	log_verb("Initialized ACPI!\n");
	while(1){
		// Infinite loop
	}
}
