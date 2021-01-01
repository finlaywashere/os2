#include <arch/x86_64/tty.h>
#include <mem/pmm.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/acpi.h>
#include <log.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/driver/pci.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <arch/x86_64/driver/timer/timer.h>
#include <arch/x86_64/driver/disk/ide.h>

void kernel_start(){
	init_paging();
	init_kmalloc(1024*100); // Assume 100MiB of memory
	init_ttys(1);
	log_debug("Initialized core kernel!\n");
	init_gdt();
	log_debug("Initialized GDT!\n");
	init_idt();
	log_debug("Initialized IDT!\n");
	init_timers();
        log_debug("Initialized timers!\n");
	init_acpi();
	log_debug("Initialized ACPI!\n");
	init_pci();
	log_debug("Initialized PCI!\n");
	init_disks();
	log_debug("Initialized disks!\n");
	uint8_t* buffer = kmalloc_p(512); // IDE buffer
	uint8_t code = ide_ata_read(2, 1, 0, buffer);
	log_error("IDE error code: 0x");
	log_error_num(code,16);
	while(1){
		// Infinite loop
	}
}
