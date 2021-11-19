#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stddef.h>
#include <mem/vmm.h>
#include <utils.h>

struct pci_function{
	uint8_t bus_num;
	uint8_t device_num;
	uint8_t function_num;
	uint16_t vendor;
	uint16_t device;
	uint16_t command;
	uint16_t status;
	uint8_t revision;
	uint8_t prog_if;
	uint8_t subclass_code;
	uint8_t class_code;
	uint8_t cache_line_size;
	uint8_t latency_timer;
	uint8_t header_type;
	uint8_t bist;
	uint32_t bar0;
	uint32_t bar1;
	uint32_t bar2;
	uint32_t bar3;
	uint32_t bar4;
	uint32_t bar5;
	uint32_t cardbus_pointer;
	uint16_t subsystem_vendor;
	uint16_t subsystem_id;
	uint32_t expansion_rom;
	uint8_t capabilities;
	uint8_t reserved1;
	uint16_t reserved2;
	uint32_t reserved3;
	uint8_t interrupt_line;
	uint8_t interrupt_pin;
	uint8_t min_grant;
	uint8_t max_latency;
	uint8_t exists;
};

typedef struct pci_function pci_function_t;

struct pci_device{
	pci_function_t functions[8];
	uint8_t exists;
};
typedef struct pci_device pci_device_t;

struct pci_bus{
	pci_device_t devices[32];
	uint8_t exists;
};
typedef struct pci_bus pci_bus_t;

struct pci{
	pci_bus_t busses[256];
};
typedef struct pci pci_t;

void init_pci();
uint16_t pci_config_read(uint8_t bus, uint8_t device, uint8_t function, uint8_t offset);
pci_function_t* get_function_by_class(uint8_t class_code, uint8_t subclass_code, uint8_t prog_if);

#endif
