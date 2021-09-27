#include <arch/x86_64/driver/disk/ahci.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <arch/x86_64/driver/pci.h>
#include <arch/x86_64/paging.h>
#include <log.h>
#include <utils.h>

hba_mem_t* ahci_mem;

int ahci_get_type(hba_port_t *port){
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0xF;
	uint8_t det = ssts & 0xF;
	
	if(det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if(ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
	return AHCI_DEV_SATA;
}

uint8_t ahci_write_disk(disk_t* disk, uint64_t lba, uint64_t count, uint8_t* buffer){
	//TODO: Implement this
}
uint8_t ahci_read_disk(disk_t* disk, uint64_t lba, uint64_t count, uint8_t* buffer){
	//TODO: Implement this
}
void ahci_probe_port(){
	uint32_t pi = ahci_mem->pi;
	for(int i = 0; i < 32; i++){
		if(pi & (1 << i)){
			int type = ahci_get_type(&ahci_mem->ports[i]);
			if(type == AHCI_DEV_SATA){
				log_warn("Found SATA AHCI device!\n");
				disk_t disk;
				disk.name[0] = 's';
				disk.name[1] = 'a';
				disk.name[2] = 't';
				disk.name[3] = 'a';
				disk.name[4] = '0'+i;
				for(int i1 = 5; i1 < 16; i1++){
					disk.name[i1] = 0x0;
				}
				disk.flags = 1;
				disk.type = 0;
				disk.identifier = i;
				disk.read_disk = &ahci_read_disk;
				disk.write_disk = &ahci_write_disk;
				register_disk(disk);

			}
		}
	}
}
void start_cmd(hba_port_t *port){
	while(port->cmd & HBA_PxCMD_CR);

	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}
void stop_cmd(hba_port_t* port){
	port->cmd &= ~HBA_PxCMD_ST;
	port->cmd &= ~HBA_PxCMD_FRE;
	while(1){
		if(port->cmd & HBA_PxCMD_FR)
			continue;
		if(port->cmd & HBA_PxCMD_CR)
			continue;
		break;
	}
}

void ahci_rebase(){
	uint64_t size = 32*(1024+256+256*32);
	uint64_t ahci_base = kmalloc_p(size);
	memset((uint8_t*)ahci_base,0,size);
	ahci_base = get_physical_addr(ahci_base);
	for(int i = 0; i < 32; i++){
		hba_port_t *port = &ahci_mem->ports[i];
		stop_cmd(port);
		
		port->clb = ahci_base;
		port->clbu = 0;
		ahci_base += 1024;
		
		port->fb = ahci_base;
		port->fbu = 0;
		ahci_base += 256;

		hba_cmd_header_t *cmd_header = (hba_cmd_header_t*) phys_to_virt(port->clb);
		for(int i = 0; i < 32; i++){
			cmd_header[i].prdtl = 8;
			cmd_header[i].ctba = ahci_base;
			cmd_header[i].ctbau = 0;
			ahci_base += 256;
		}
		start_cmd(port);
	}
}
void init_ahci(){
	pci_function_t* controller = get_function_by_class(0x1, 0x6, 1);
	if(controller == 0x0){
		log_error("No AHCI controller found!");
		return;
	}
	uint64_t base = phys_to_virt(controller->bar5);
	ahci_mem = (hba_mem_t*) base;
	ahci_rebase();
	ahci_probe_port();
}
