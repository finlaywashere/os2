#include <arch/x86_64/driver/pci.h>

void init_pci(){
	
}

uint16_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset){
	uint32_t address = (uint32_t)(bus<<16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	outd(0xCF8,address);
	uint16_t tmp = (uint16_t)((ind(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return tmp;
}

