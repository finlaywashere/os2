#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64/tty.h>
#include <mem/pmm.h>
#include <arch/x86_64/paging.h>

void kernel_start(){
	init_paging();
	init_kmalloc(1024*100); // Assume 100MiB of memory
	init_ttys(3);
	tty_writestring(2,"Successfully booted kernel!");
	tty_writestring(1,"Test1234");
	uint64_t i = 0;
	int mode = 0;
	while(1){
		i++;
		if(i >= 0xFFFFFFFFFFFFFFFF){
			i = 0;
			if(mode){
				tty_copy(1,0);
				mode = 0;
			}else{
				tty_copy(2,0);
				mode = 1;
			}
		}
	}
}
