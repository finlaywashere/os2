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
#include <fs/fs.h>
#include <elf.h>

void kernel_start(){
	init_paging();
	init_kmalloc();
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
	init_filesystems();
	log_debug("Initialized filesystems!\n");
	page_table_t* dst = (page_table_t*) kmalloc_p(sizeof(page_table_t));
	uint64_t entry_point = load_elf("/test.elf", dst);
	log_error_num(entry_point,16);
	set_page_directory(dst);
	asm volatile("jmp 0x1000");
	while(1){
		// Infinite loop
	}
}
