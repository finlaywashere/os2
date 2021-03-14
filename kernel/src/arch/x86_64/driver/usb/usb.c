#include <arch/x86_64/driver/usb/usb.h>

uhci_controller_t *uhci_controller;
uhci_frame_t *uhci_frames;

void read_controller(){
	uint16_t port = uhci_controller->port;
	uhci_controller->command = inw(port);
	uhci_controller->status = inw(port+0x2);
	uhci_controller->interrupt = inw(port+0x4);
	uhci_controller->frame_number = inw(port+0x6);
	uhci_controller->frame_addr = ind(port+0x8);
	uhci_controller->frame_modify = inb(port+0xC);
	uhci_controller->port1_status = (device_status_t) inw(port+0x10);
	uhci_controller->port2_status = (device_status_t) inw(port+0x12);
}
void write_controller(){
	uint16_t port = uhci_controller->port;
	outw(port,uhci_controller->command);
	outw(port+0x2,uhci_controller->status);
	outw(port+0x4,uhci_controller->interrupt);
	outw(port+0x6,uhci_controller->frame_number);
	outd(port+0x8,uhci_controller->frame_addr);
	outw(port+0xC,uhci_controller->frame_modify);
	outw(port+0x10,(uint16_t)(uhci_controller->port1_status));
	outw(port+0x12,(uint16_t)(uhci_controller->port2_status));
}

void init_usb(){
	pci_function_t* pci_controller = get_function_by_class(0xC, 0x3, 0x0); // Get controller by class
	uint64_t port = pci_controller->bar4 & 0xFFFFFFFC;
	uhci_controller = (uhci_controller_t*) kmalloc_p(sizeof(uhci_controller_t));
	uhci_controller->port = port;
	read_controller();
	uhci_frames = (uhci_frame_t*) (0xffffff8000000000 + ((uint64_t) uhci_controller->frame_addr));
	
}
