#include <arch/x86_64/arch.h>
#include <fs/devfs.h>

void init_arch(){
	init_paging();
    init_kmalloc();
    init_ttys(1);
    init_gdt();
    init_idt();
	
	init_devfs(); // Needed to add driver data n stuff to devfs

    init_acpi();
	init_drivers();
}
