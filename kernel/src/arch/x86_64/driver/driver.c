#include <arch/x86_64/driver/driver.h>

void init_drivers(){
	init_keyboard();
    init_timers();
    init_pci();
    init_disks();
	init_vga();
}
