#include <arch/x86_64/acpi.h>
#include <arch/x86_64/tty.h>
rsdp_t* rsdp;
void init_acpi(){
	uint64_t rsdp_addr = 0;
	uint16_t* ebda_addr = (uint16_t*)0xffffff800000040e;
	uint64_t ebda_base = *ebda_addr + 0xffffff8000000000; // BIOS extended data area
	for(int i = 0; i <= 1024; i += 16){
		uint8_t* ebda = (uint8_t*)(ebda_base+i);
		if(memcmp(ebda,"RSD PTR ",8)){
			// Found ACPI table
			tty_writestring(0,"Found ACPI table in EBDA!");
			rsdp_addr = (uint64_t) ebda;
			break;
		}
	}
	if(rsdp_addr == 0){
		for(uint64_t addr = 0xffffff80000E0000; addr <= 0xffffff80000FFFFF; addr += 16){
			uint8_t* ebda = (uint8_t*) addr;
                	if(memcmp(ebda,"RSD PTR ",8)){
                        	// Found ACPI table
                        	tty_writestring(0,"Found ACPI table in BIOS data area!");
                        	rsdp_addr = (uint64_t) ebda;
                        	break;
                	}
		}
	}
	rsdp = (rsdp_t*)rsdp_addr;
}
