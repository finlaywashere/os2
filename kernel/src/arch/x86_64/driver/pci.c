#include <arch/x86_64/driver/pci.h>
#include <fs/devfs.h>
#include <fs/fs.h>

pci_t* pci;

uint64_t pci_dir_entries(fs_node_t* file){
	uint8_t flags = file->inode >> 24;
	if(flags & 1){
		uint64_t count = 0;
		for(int i = 0; i < 256; i++){
			if(pci->busses[i].exists)
				count++;
		}
		return count;
	}else if(flags & 2){
		pci_bus_t* bus = &pci->busses[file->inode & 0xFF];
		uint64_t count = 0;
		for(int i = 0; i < 32; i++){
			if(bus->devices[i].exists)
				count++;
		}
		return count;
	}else if(flags & 4){
		int bus = file->inode & 0xFF;
		int device = file->inode & 0xFF00;
		pci_device_t* dev = &pci->busses[bus].devices[device];
		uint64_t count = 0;
		for(int i = 0; i < 8; i++){
			if(dev->functions[i].exists)
				count++;
		}
		return count;
	}else{
		return 5;
	}
}
uint8_t pci_read_file(fs_node_t* file, uint64_t offset, uint64_t count, uint8_t* buffer){
	uint8_t bus = file->inode & 0xFF;
	uint8_t device = file->inode & 0xFF00;
	uint8_t function = file->inode & 0xFF0000;
	uint8_t id = file->inode >> 24;
	pci_function_t *func = &pci->busses[bus].devices[device].functions[function];
	if(id == 0){
		int_to_str(buffer, func->class_code, 10);
	}else if(id == 1){
		int_to_str(buffer, func->subclass_code, 10);
	}else if(id == 2){
		int_to_str(buffer, func->prog_if, 10);
	}else if(id == 3){
		buffer[0] = '0';
		buffer[1] = 'x';
		int_to_str(&buffer[2], func->vendor, 16);
	}else if(id == 4){
		buffer[0] = '0';
		buffer[1] = 'x';
		int_to_str(&buffer[2], func->device, 16);
	}
}
void pci_create_file(fs_node_t* file, char* name, uint64_t inode){
	file->type = 1;
	strcpy(name,&file->name);
	file->inode = inode;
	file->read_file = &pci_read_file;
	file->length = 10; // Max of 10 bytes in a file
	file->exists = 1;
}
uint8_t pci_read_dir(fs_node_t* file, fs_node_t* buffer){
	uint64_t len = pci_dir_entries(file);
	uint8_t flags = file->inode >> 24;
	uint8_t bus = file->inode & 0xFF;
	uint8_t device = file->inode & 0xFF00;
	uint8_t function = file->inode & 0xFF0000;
	if(flags & 1){
		int index = 0;
		for(int i = 0; i < 256; i++){
			if(pci->busses[i].exists){
				buffer[index].inode = i | (2 << 24);
				int_to_str(&buffer[index].name, i, 10);
				buffer[index].type = 0;
				buffer[index].read_dir = &pci_read_dir;
				buffer[index].dir_entries = &pci_dir_entries;
				buffer[index].exists = 1;
				index++;
			}
		}
		return 0;
	}else if(flags & 2){
		int index = 0;
		for(int i = 0; i < 32; i++){
			if(pci->busses[bus].devices[i].exists){
				buffer[index].inode = i | (4 << 24);
				int_to_str(&buffer[index].name, i, 10);
				buffer[index].type = 0;
				buffer[index].read_dir = &pci_read_dir;
				buffer[index].dir_entries = &pci_dir_entries;
				buffer[index].exists = 1;
				index++;
			}
		}
		return 0;
	}else if(flags & 4){
		int index = 0;
		for(int i = 0; i < 8; i++){
			if(pci->busses[bus].devices[device].functions[i].exists){
				buffer[index].inode = i | (8 << 24);
				int_to_str(&buffer[index].name, i, 10);
				buffer[index].type = 0;
				buffer[index].read_dir = &pci_read_dir;
				buffer[index].dir_entries = &pci_dir_entries;
				buffer[index].exists = 1;
				index++;
			}
		}
		return 0;
	}else{
		pci_function_t *func = &pci->busses[bus].devices[device].functions[function];
		uint64_t new_inode = bus | (device << 8) | (function << 16);
		pci_create_file(&buffer[0],"class",new_inode);
		pci_create_file(&buffer[1],"subclass",new_inode | (1 << 24));
		pci_create_file(&buffer[2],"progif",new_inode | (2 << 24));
		pci_create_file(&buffer[3],"vendor",new_inode | (3 << 24));
		pci_create_file(&buffer[4],"device",new_inode | (4 << 24));
		return 0;
	}
}

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
	func->exists = 1;

	pci->busses[bus].exists = 1;
	pci->busses[bus].devices[device].exists = 1;
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

void init_pci(){
	pci = (pci_t*) kmalloc_p(sizeof(pci_t));
	fs_node_t* dir = (fs_node_t*) kmalloc_pa(sizeof(fs_node_t),1);
	dir->exists = 1;
	strcpy("pci",&dir->name);
	dir->type = 0;
	dir->dir_entries = &pci_dir_entries;
	dir->read_dir = &pci_read_dir;
	dir->inode = 1 << 24;
	register_folder(dir);
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

