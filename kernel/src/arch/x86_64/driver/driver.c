#include <arch/x86_64/driver/driver.h>

void init_drivers(){
	init_pci();
	init_timers();
	init_disks();
	init_usb();
	init_keyboard();
}
