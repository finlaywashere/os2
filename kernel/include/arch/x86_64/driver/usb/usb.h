#ifndef USB_H
#define USB_H

#include <stdint.h>
#include <stddef.h>

struct uhci_io{
	uint16_t cmd;
	uint16_t status;
	uint16_t ie;
	uint16_t frame_num;
	uint32_t frame_list_base;
	uint8_t start_frame_modify;
	uint16_t port1_ctrl;
	uint16_t port2_ctrl;
};

typedef struct uhci_io uhci_io_t;

struct uhci_descriptor{
	uint32_t next_descriptor;
	uint32_t status;
	uint32_t packet_header;
	uint32_t buffer_address;
	uint64_t system_use[2];
}__attribute__((packed));

typedef struct uhci_descriptor uhci_descriptor_t;

void init_usb();

#endif
