#include <arch/x86_64/driver/disk/ide.h>

char* ide_buf;
char irq_invoked = 0;
char atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void init_ide(){
	pci_function_t* controller = get_function_by_class(1, 1, 0x80); // Look for PCI device with class code 1, and subclass 1
	
	// Some of this should probably be moved to the pci code but idc
	ide_buf = kmalloc_p(2048);
	
	channels[ATA_PRIMARY].io_base = (controller->bar0 & 0xFFFFFFFC) + 0x1F0 * (!controller->bar0);
	channels[ATA_PRIMARY].control_base = (controller->bar1 & 0xFFFFFFFC) + 0x3F6 * (!controller->bar1);
	channels[ATA_SECONDARY].io_base = (controller->bar2 & 0xFFFFFFFC) + 0x170 * (!controller->bar2);
	channels[ATA_SECONDARY].control_base = (controller->bar3 & 0xFFFFFFFC) + 0x376 * (!controller->bar3);
	channels[ATA_PRIMARY].no_interrupt = (controller->bar4 & 0xFFFFFFFC);
	channels[ATA_SECONDARY].no_interrupt = (controller->bar4 & 0xFFFFFFFC) + 0x8;
	
	// Disable IRQs
	ide_write(ATA_PRIMARY , ATA_REG_CONTROL, 2);
	ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);
	
	for(int i = 0; i < 2; i++){
		for(int j = 0; j < 2; j++){
			int count = i*2+j;
			devices[count].exists = 0; // Assume it doesn't exist
			ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // If bit 4 is set then the secondary drive is selected
			sleep(1);
			ide_write(i,ATA_REG_COMMAND,ATA_CMD_IDENTIFY); // Send identify command
			sleep(1);
			if(ide_read(i,ATA_REG_STATUS) == 0) continue; // Device doesn't exist if status is 0
			uint8_t error = 0;
			while(1){
				uint8_t status = ide_read(i,ATA_REG_STATUS);
				if((status & ATA_SR_ERR)){
					error = 1;
					break;
				}
				if(!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)){
					break;
				}
			}
			if(error != 0){
				// Non ATA devices not supported so skip it
				continue;
			}
			ide_read_buffer(i,ATA_REG_DATA, (uint32_t*) ide_buf, 128);
			devices[count].exists = 1;
			devices[count].type = IDE_ATA;
			devices[count].channel = i;
			devices[count].drive = j;
			devices[count].signature = *((uint16_t*)(ide_buf + ATA_IDENT_DEVICETYPE));
			devices[count].capabilities = *((uint16_t*)(ide_buf + ATA_IDENT_CAPABILITIES));
			devices[count].command_set = *((uint32_t*)(ide_buf + ATA_IDENT_COMMANDSETS));
			if(devices[count].command_set & (1 << 26))
				devices[count].size = *((uint32_t*)(ide_buf+ATA_IDENT_MAX_LBA_EXT));
			else
				devices[count].size = *((uint32_t*)(ide_buf+ATA_IDENT_MAX_LBA));
			for(int k = 0; k < 40; k+= 2){
				devices[count].model[k] = ide_buf[ATA_IDENT_MODEL+k+1];
				devices[count].model[k+1] = ide_buf[ATA_IDENT_MODEL+k];
			}
			devices[count].model[40] = 0x0; // Null terminator
			log_warn("Disk model: ");
			log_warn(devices[count].model);
			log_warn("\n");
		}
	}
}
uint8_t ide_read(uint8_t channel, uint8_t reg){
	uint8_t result;
	if(reg > 0x7 && reg < 0x0C)
		ide_write(channel,ATA_REG_CONTROL,0x80 | channels[channel].no_interrupt);
	if(reg < 0x8)
		result = inb(channels[channel].io_base+reg);
	else if(reg < 0xC)
		result = inb(channels[channel].io_base+reg-0x6);
	else if(reg < 0xE)
		result = inb(channels[channel].control_base+reg-0xA);
	else if(reg < 0x16)
		result = inb(channels[channel].bus_master_ide+reg-0xE);
	if(reg > 0x7 && reg < 0xC)
		ide_write(channel,ATA_REG_CONTROL, channels[channel].no_interrupt);
	return result;
}
void ide_write(uint8_t channel, uint8_t reg, uint8_t data){
	if(reg > 0x7 && reg < 0x0C)
                ide_write(channel,ATA_REG_CONTROL,0x80 | channels[channel].no_interrupt);
        if(reg < 0x8)
                outb(channels[channel].io_base+reg,data);
        else if(reg < 0xC)
                outb(channels[channel].io_base+reg-0x6,data);
        else if(reg < 0xE)
                outb(channels[channel].control_base+reg-0xA,data);
        else if(reg < 0x16)
                outb(channels[channel].bus_master_ide+reg-0xE,data);
        if(reg > 0x7 && reg < 0xC)
                ide_write(channel,ATA_REG_CONTROL, channels[channel].no_interrupt);

}
void ide_read_buffer(uint8_t channel, uint8_t reg, uint32_t* buffer, uint32_t count){
	if(reg > 0x7 && reg < 0x0C)
                ide_write(channel,ATA_REG_CONTROL,0x80 | channels[channel].no_interrupt);
	for(uint32_t i = 0; i < count; i++){
        	if(reg < 0x8)
                	buffer[i] = ind(channels[channel].io_base+reg);
        	else if(reg < 0xC)
                	buffer[i] = ind(channels[channel].io_base+reg-0x6);
        	else if(reg < 0xE)
       	         	buffer[i] = ind(channels[channel].control_base+reg-0xA);
        	else if(reg < 0x16)
         	       buffer[i] = ind(channels[channel].bus_master_ide+reg-0xE);
	}
        if(reg > 0x7 && reg < 0xC)
                ide_write(channel,ATA_REG_CONTROL, channels[channel].no_interrupt);
}
uint8_t ide_poll(uint8_t channel, uint8_t error_check){
	for(int i = 0; i < 4; i++)
		ide_read(channel,ATA_REG_ALTSTATUS); // Reading altstatus wastes 100ns each
	while((ide_read(channel,ATA_REG_STATUS) & ATA_SR_BSY) == 1) ;
	
	if(error_check){
		uint8_t state = ide_read(channel,ATA_REG_STATUS);
		if((state & ATA_SR_ERR) == 1)
			return 2;
		if((state & ATA_SR_DF) == 1)
			return 1;
		if((state & ATA_SR_DRQ) == 0)
			return 3;
	}
	return 0;
}
