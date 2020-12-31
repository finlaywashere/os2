#include <arch/x86_64/driver/pci.h>
#include <log.h>

pci_t* pci;

void check_function(uint8_t bus, uint8_t device, uint8_t function){
	uint16_t vendor = pci_config_read(bus,device,function,0);
	uint16_t device_id = pci_config_read(bus,device,function,2);
	uint16_t command = pci_config_read(bus,device,function,4);
	uint16_t status = pci_config_read(bus,device,function,6);
	uint8_t revision = (uint8_t) pci_config_read(bus,device,function,8);
	uint8_t prog_if = pci_config_read(bus,device,function,8) >> 8;
	uint8_t subclass_code = (uint8_t) pci_config_read(bus,device,function,10);
	uint8_t class_code = pci_config_read(bus,device,function,10) >> 8;
	uint8_t cache_line_size = (uint8_t) pci_config_read(bus,device,function,12);
	uint8_t latency_timer = pci_config_read(bus,device,function,12) >> 8;
	uint8_t header = (uint8_t) pci_config_read(bus,device,function,14);
	uint8_t bist = pci_config_read(bus,device,function,14) >> 8;
	uint32_t bar0 = pci_config_read(bus,device,function,16) | (pci_config_read(bus,device,function,18) << 16);
	uint32_t bar1 = pci_config_read(bus,device,function,20) | (pci_config_read(bus,device,function,22) << 16);
	uint32_t bar2 = pci_config_read(bus,device,function,24) | (pci_config_read(bus,device,function,26) << 16);
	uint32_t bar3 = pci_config_read(bus,device,function,28) | (pci_config_read(bus,device,function,30) << 16);
	uint32_t bar4 = pci_config_read(bus,device,function,32) | (pci_config_read(bus,device,function,34) << 16);
	uint32_t bar5 = pci_config_read(bus,device,function,36) | (pci_config_read(bus,device,function,38) << 16);
	uint32_t cardbus = pci_config_read(bus,device,function,40) | (pci_config_read(bus,device,function,42) << 16);
	uint16_t subsystem_vendor = pci_config_read(bus,device,function,44);
	uint16_t subsystem = pci_config_read(bus,device,function,46);
	uint32_t expansion_rom = pci_config_read(bus,device,function,48) | (pci_config_read(bus,device,function,50) << 16);
	uint8_t capabilities = (uint8_t) pci_config_read(bus,device,function,52);
	uint8_t interrupt_line = (uint8_t) pci_config_read(bus,device,function,60);
	uint8_t interrupt_pin = pci_config_read(bus,device,function,60) >> 8;
	uint8_t min_grant = (uint8_t) pci_config_read(bus,device,function,62);
	uint8_t max_latency  = pci_config_read(bus,device,function,62) >> 8;
	pci_function_t* func = &(pci->busses[bus].devices[device].functions[function]);
	
	func->bus_num = bus;
	func->device_num = device;
	func->function_num = function;
	
	func->vendor = vendor;
	func->device = device_id;
	func->command = command;
	func->status = status;
	func->revision = revision;
	func->prog_if = prog_if;
	func->subclass_code = subclass_code;
	func->class_code = class_code;
	func->cache_line_size = cache_line_size;
	func->latency_timer = latency_timer;
	func->header_type = header;
	func->bist = bist;
	func->bar0 = bar0;
	func->bar1 = bar1;
	func->bar2 = bar2;
	func->bar3 = bar3;
	func->bar4 = bar4;
	func->bar5 = bar5;
	func->cardbus_pointer = cardbus;
	func->subsystem_vendor = subsystem_vendor;
	func->subsystem_id = subsystem;
	func->expansion_rom = expansion_rom;
	func->capabilities = capabilities;
	func->interrupt_line = interrupt_line;
	func->interrupt_pin = interrupt_pin;
	func->min_grant = min_grant;
	func->max_latency = max_latency;
}

void check_device(uint8_t bus, uint8_t device){
	uint8_t function = 0;
	uint16_t vendor = pci_config_read(bus,device,function,0);
	if(vendor == 0xFFFF) return;
	check_function(bus,device,function);
	uint8_t header = (uint8_t) pci_config_read(bus,device,function,14);
	if((header & 0x80) != 0){
		for(function = 1; function < 8; function++){
			vendor = pci_config_read(bus,device,function,0);
			if(vendor != 0xFFFF)
				check_function(bus,device,function);
		}
	}
}
void check_bus(uint8_t busNum){
	for(uint8_t device = 0; device < 32; device++){
		check_device(busNum,device);
	}
}

void print_pci_info(){
	for(int bus = 0; bus < 256; bus++){
		for(int device = 0; device < 32; device++){
			for(int function = 0; function < 8; function++){
				pci_function_t func = pci->busses[bus].devices[device].functions[function];
				if(func.class_code == 0xFF || func.class_code == 0x0)
					continue;
				log_debug("Found PCI device, 0x");
		                uint64_t len = numlen(func.class_code,16);
        		        char* buf = (char*) kmalloc_p(len);
        		        int_to_str(buf,func.class_code,16);
        		        log_debugl(buf,len);
                		kfree_p((void*) buf,len);
                		log_debug(" 0x");
				
		                len = numlen(func.subclass_code,16);
                		buf = (char*) kmalloc_p(len);
                		int_to_str(buf,func.subclass_code,16);
                		log_debugl(buf,len);
                		kfree_p((void*) buf,len);
		                log_debug(" 0x");
				
                		len = numlen(func.prog_if,16);
 		                buf = (char*) kmalloc_p(len);
                  		int_to_str(buf,func.prog_if,16);
		                log_debugl(buf,len);
                		kfree_p((void*) buf,len);
				
		                log_debug("!\n");
			}
		}
	}
}

void init_pci(){
	pci = (pci_t*) kmalloc_p(sizeof(pci_t));
	check_bus(0);
}
pci_function_t* get_function_by_class(uint8_t class_code, uint8_t subclass_code, uint8_t prog_if){
	// If any values are -1 then don't check them
	for(int bus = 0; bus < 256; bus++){
                for(int device = 0; device < 32; device++){
                        for(int function = 0; function < 8; function++){
                                pci_function_t* func = &pci->busses[bus].devices[device].functions[function];
				if(func->class_code == 0xFF || func->class_code == 0)
                                        continue;
				if(func->class_code == class_code || class_code == -1){
					if(func->subclass_code == subclass_code || subclass_code == -1){
						if(func->prog_if == prog_if || prog_if == -1)
							return func;
					}
				}
			}
		}
	}
	return 0x0;
}

uint16_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset){
	uint32_t address = (uint32_t)(bus<<16) | (device << 11) | (function << 8) | (offset & 0xFC) | (1 << 31);
	outd(0xCF8,address);
	uint16_t tmp = (uint16_t)((ind(0xCFC) >> ((offset & 2) * 8)) & 0xFFFF);
	return tmp;
}

