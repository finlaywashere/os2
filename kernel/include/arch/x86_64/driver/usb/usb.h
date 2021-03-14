#ifndef USB_H
#define USB_H

#include <stdint.h>
#include <stddef.h>
#include <arch/x86_64/driver/pci.h>
#include <utils.h>

struct uhci_frame{
	int valid : 1;
	int td : 1;
	int reserved : 2;
	int address : 28;
}__attribute__((packed));

typedef struct uhci_frame uhci_frame_t;

struct uhci_descriptor{
	uint32_t next_descriptor;
	uint32_t status;
	uint32_t header;
	uint32_t buffer;
	uint8_t reserved[16];
}__attribute__((packed));
typedef struct uhci_descriptor uhci_descriptor_t;

struct device_status{
	int connected : 1;
	int connected_changed : 1;
	int enabled : 1;
	int enabled_changed : 1;
	int line_status : 2;
	int resume_detected : 1;
	int reserved : 1;
	int low_speed : 1;
	int reset : 1;
	int reserved2: 2;
	int suspend : 1;
	int reserved3 : 3;
}__atrribute__((packed));

typedef struct device_status device_status_t;

struct uhci_controller{
    uint16_t port;
    uint16_t command;
    uint16_t status;
    uint16_t interrupt;
    uint16_t frame_number;
    uint32_t frame_addr;
    uint8_t frame_modify;
    device_status_t port1_status;
    device_status_t port2_status;
};
typedef struct uhci_controller uhci_controller_t;


void init_usb();

#endif
