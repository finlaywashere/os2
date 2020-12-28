#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64/tty.h>
#include <mem/pmm.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/acpi.h>

void kernel_start(){
	init_paging();
	init_kmalloc(1024*100); // Assume 100MiB of memory
	init_ttys(1);
	init_acpi();
	while(1){
		// Infinite loop
	}
}
