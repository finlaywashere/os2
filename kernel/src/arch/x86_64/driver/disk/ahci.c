#include <arch/x86_64/driver/disk/ahci.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <arch/x86_64/driver/pci.h>
#include <arch/x86_64/paging.h>
#include <log.h>
#include <utils.h>

hba_mem_t* ahci_mem;

int ahci_get_type(volatile hba_port_t *port){
	uint32_t ssts = port->ssts;

	uint8_t ipm = (ssts >> 8) & 0xF;
	uint8_t det = ssts & 0xF;
	
	if(det != HBA_PORT_DET_PRESENT)
		return AHCI_DEV_NULL;
	if(ipm != HBA_PORT_IPM_ACTIVE)
		return AHCI_DEV_NULL;
	return AHCI_DEV_SATA;
}
int __attribute__ ((noinline)) ahci_find_cmdslot(volatile hba_port_t* port){
	uint32_t slots = (port->sact | port->ci);
	for(int i = 0; i < 32; i++){
		if((slots & (1 << i)) == 0)
			return i;
	}
	panic("Error: Failed to find AHCI command slot!");
	return -1;
}

int ahci_cmd(volatile hba_port_t* port, uint64_t lba, uint32_t count, uint16_t* buffer, int write){
	port->is = (uint32_t) -1; // Clear interrupt bits
	int spin = 0;
	int slot = ahci_find_cmdslot(port);
	if(slot == -1)
		return 0;
	uint64_t cmd_address = ((uint64_t) port->clb) | (((uint64_t) port->clbu) << 32);
	hba_cmd_header_t *cmdheader = (hba_cmd_header_t*) phys_to_virt(cmd_address);
	cmdheader += slot;
	cmdheader->cfl = sizeof(fis_reg_h2d_t)/sizeof(uint32_t);
	cmdheader->w = write;
	cmdheader->c = 1;
	cmdheader->prdtl = (uint16_t)((count-1)>>4) + 1;
	uint64_t tbl_address = ((uint64_t) cmdheader->ctba) | (((uint64_t) cmdheader->ctbau) << 32);
	hba_cmd_tbl_t* cmdtbl = (hba_cmd_tbl_t*) phys_to_virt(tbl_address);
	memset(cmdtbl,0,sizeof(hba_cmd_tbl_t) + (cmdheader->prdtl-1)*sizeof(hba_prdt_entry_t));
	
	uint16_t* buf = (uint16_t*) get_physical_addr(buffer);
	int i = 0;
	for(; i < cmdheader->prdtl-1; i++){
		cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
		cmdtbl->prdt_entry[i].dbc = 8*1024-1; // 8K bytes
		cmdtbl->prdt_entry[i].i = 1;
		buf += 4*1024; // 4K words
		count -= 16;
	}
	cmdtbl->prdt_entry[i].dba = (uint32_t) buf;
	cmdtbl->prdt_entry[i].dbc = (count << 9)-1; // 512 bytes per sector
	cmdtbl->prdt_entry[i].i = 1;

	fis_reg_h2d_t *cmdfis = (fis_reg_h2d_t*) (&cmdtbl->cfis);
	cmdfis->fis_type = FIS_TYPE_REG_H2D;
	cmdfis->c = 1;
	if(write == 0)
		cmdfis->command = ATA_CMD_READ_DMA_EX;
	else
		cmdfis->command = ATA_CMD_WRITE_DMA_EX;

	cmdfis->lba0 = (uint8_t) lba;
	cmdfis->lba1 = (uint8_t) (lba >> 8);
	cmdfis->lba2 = (uint8_t) (lba >> 16);
	cmdfis->lba3 = (uint8_t) (lba >> 24);
	cmdfis->lba4 = (uint8_t) (lba >> 32);
	cmdfis->lba5 = (uint8_t) (lba >> 40);

	cmdfis->countl = count & 0xFF;
	cmdfis->counth = (count >> 8) & 0xFF;

	cmdfis->device = 1 << 6; // LBA mode

	while((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000){
		spin++;
	}
	if(spin == 1000000){
		panic("AHCI port is hung!");
		return 0;
	}
	port->ci = 1<<slot;

	while(1){
		if((port->ci & (1<<slot)) == 0)
			break;
		if(port->is & HBA_PxIS_TFES){
			log_error("AHCI read/write disk error!");
			return 0;
		}
	}
	if(port->is & HBA_PxIS_TFES){
		log_error("AHCI read/write disk error!");
		return 0;
	}
	while(port->ci != 0);
	return 1;
}

uint8_t ahci_write_disk(disk_t* disk, uint64_t lba, uint64_t count, uint8_t* buffer){
	volatile hba_port_t* port = &ahci_mem->ports[disk->identifier];
    return ahci_cmd(port,lba,count,(uint16_t*) buffer, 1);
}
uint8_t ahci_read_disk(disk_t* disk, uint64_t lba, uint64_t count, uint8_t* buffer){
	volatile hba_port_t* port = &ahci_mem->ports[disk->identifier];
	return ahci_cmd(port,lba,count,(uint16_t*) buffer, 0);
}
void ahci_probe_port(){
	uint32_t pi = ahci_mem->pi;
	for(int i = 0; i < 32; i++){
		if(pi & (1 << i)){
			hba_port_t* port = &ahci_mem->ports[i];
			port->ie = 0;
			port->cmd |= 1;
			int type = ahci_get_type(port);
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
void start_cmd(volatile hba_port_t *port){
	while(port->cmd & HBA_PxCMD_CR);
	
	port->cmd |= 1 << 3; // Set CLO bit
	sleep(1);
	port->cmd &= ~(1 << 3); // Clear CLO bit
	port->cmd |= HBA_PxCMD_FRE;
	port->cmd |= HBA_PxCMD_ST;
}
void stop_cmd(volatile hba_port_t* port){
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
	int pi = ahci_mem->pi;
	for(int i = 0; i < 32; i++){
		if(!(pi & (1 << i)))
			continue;
		volatile hba_port_t *port = &ahci_mem->ports[i];
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
		ahci_base += 256*3; // CLB musut be 1024KiB aligned
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
	//ahci_mem->bohc |= 1 << 1;
	//ahci_rebase();
	//ahci_mem->ghc |= 1 | (1 << 32);
	/*for(int i = 0; i < 32; i++){
		if(ahci_mem->pi & i){
			stop_cmd(&ahci_mem->ports[i]);
		}
	}
	ahci_mem->ghc |= 1 | (1 << 31);
	int spin = 0;
	while(ahci_mem->ghc & 1){
		spin++;
		if(spin > 100000){
			panic("Timed out resetting AHCI controller");
		}
	}*/
	//ahci_mem->ghc |= (1 << 1) | (1 << 31);
	/*for(int i = 0; i < 32; i++){
		if(ahci_mem->pi & i){
			start_cmd(&ahci_mem->ports[i]);
		}
	}*/
	ahci_probe_port();
}
