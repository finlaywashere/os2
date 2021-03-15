#include <arch/x86_64/arch.h>

void init_arch(){
	init_paging();
    init_kmalloc();
    init_ttys(1);
    init_gdt();
    init_idt();
    init_acpi();
	init_drivers();
}
