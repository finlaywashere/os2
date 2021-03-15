#include <arch/x86_64/acpi.h>
acpi_header_t* rsdt;
void init_acpi(){
	uint64_t rsdp_addr = 0;
	uint16_t* ebda_addr = (uint16_t*)0xffffff800000040e;
	uint64_t ebda_base = *ebda_addr + 0xffffff8000000000; // BIOS extended data area
	for(int i = 0; i <= 1024; i += 16){
		uint8_t* ebda = (uint8_t*)(ebda_base+i);
		if(memcmp(ebda,(uint8_t*)"RSD PTR ",8)){
			// Found ACPI table
			rsdp_addr = (uint64_t) ebda;
			break;
		}
	}
	if(rsdp_addr == 0){
		for(uint64_t addr = 0xffffff80000E0000; addr <= 0xffffff80000FFFFF; addr += 16){
			uint8_t* ebda = (uint8_t*) addr;
                	if(memcmp(ebda,(uint8_t*)"RSD PTR ",8)){
                        	// Found ACPI table
                        	rsdp_addr = (uint64_t) ebda;
                        	break;
                	}
		}
	}
	rsdp_t* rsdp = (rsdp_t*)rsdp_addr;
	uint8_t sum = 0;
	for(uint32_t i = 0; i < sizeof(rsdp_t); i++){
		sum += ((char*)rsdp)[i];
	}
	if(sum != 0){
		panic("RSDP header invalid!");
	}
	rsdt = (acpi_header_t*)(((uint64_t)rsdp->rsdt)+0xffffff8000000000);
	if(acpi_checksum(rsdt) == 0){
		panic("RSDT header invalid!");
	}
	uint32_t* tables = ((uint64_t)rsdt)+sizeof(acpi_header_t);
	uint32_t numTables = (rsdt->length-sizeof(acpi_header_t))/sizeof(uint32_t);
	for(uint32_t i = 0; i < numTables; i++){
		uint64_t addr = ((uint64_t)tables[i])+0xffffff8000000000;
		acpi_header_t* table = (acpi_header_t*) addr;
		if(acpi_checksum(table) == 0){
			log_error("Found invalid ACPI table, name: ");
			panicl(table->sig,4);
		}
	}
}
int acpi_checksum(acpi_header_t* header){
	uint8_t sum = 0;
	for(uint32_t i = 0; i < header->length; i++){
		sum += ((uint8_t*)header)[i];
	}
	return sum == 0;
}
acpi_header_t* get_acpi_table(char* sig){
	uint32_t* tables = ((uint64_t)rsdt)+sizeof(acpi_header_t);
        uint32_t numTables = (rsdt->length-sizeof(acpi_header_t))/sizeof(uint32_t);
	for(uint32_t i = 0; i < numTables; i++){
		acpi_header_t* table = ((uint64_t)tables[i])+0xffffff8000000000;
		if(memcmp(table->sig,sig,4) == 0){
			if(acpi_checksum(table) == 0){
                        	log_error("Found invalid ACPI table, name: ");
                        	panicl(table->sig,4);
                	}
			return table;
		}
	}
	return (acpi_header_t*) 0x0;
}
