#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64/tty.h>
#include <mem/pmm.h>

void kernel_start(){
	init_kmalloc(1024*100); // Assume 100MiB of memory
	init_ttys(1);
	tty_writestring(0,"Successfully booted kernel!");
	tty_copy(0,-1);
	while(1){
		
	}
}
