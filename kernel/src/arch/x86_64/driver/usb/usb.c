#include <arch/x86_64/driver/usb/usb.h>
#include <arch/x86_64/driver/pci.h>
#include <log.h>
#include <arch/x86_64/paging.h>
#include <utils.h>
#include <mem/vmm.h>

uint16_t io_port;

uhci_io_t *uhci_io;

uint32_t *uhci_frame_list;

void usb_read(uhci_io_t* io){
	io->cmd = inw(io_port);
	io->status = inw(io_port+0x2);
	io->ie = inw(io_port+0x4);
	io->frame_num = inw(io_port+0x6);
	io->frame_list_base = ind(io_port+0x8);
	io->start_frame_modify = inb(io_port+0xC);
	io->port1_ctrl = inw(io_port+0x10);
	io->port2_ctrl = inw(io_port+0x12);
}
void usb_write(uhci_io_t* io){
	outw(io_port,io->cmd);
	outw(io_port+0x4,io->ie);
	outd(io_port+0x8,io->frame_list_base);
	outw(io_port+0x10,io->port1_ctrl);
	outw(io_port+0x12,io->port2_ctrl);
}

void init_usb(){
	pci_function_t* controller = get_function_by_class(0xC, 0x3, 0x0); // UHCI controller
	if(controller == 0){
		panic("Failed to find UHCI controller!");
	}
	io_port = (uint16_t) controller->bar4;
	if(io_port % 2 != 0)
		io_port--; // Weird bug fix
	uhci_io = (uhci_io_t*) kmalloc_p(sizeof(uhci_io_t));
	uhci_frame_list = (uint32_t*) kmalloc_p(sizeof(uint32_t)*1024);
	usb_read(uhci_io);
	uhci_io->cmd &= ~1;
	usb_write(uhci_io);
	while(!(uhci_io->status & 1 << 5))
		usb_read(uhci_io);
	for(int i = 0; i < 1024; i++){
		uhci_frame_list[i] = 1;
	}
	uint32_t phys_addr = (uint32_t) get_physical_addr((uint64_t) uhci_frame_list);
	uhci_io->frame_list_base = phys_addr;
	uhci_io->cmd = 1;
	usb_write(uhci_io);
}
