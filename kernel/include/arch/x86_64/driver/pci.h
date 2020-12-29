#pragma once

#include <stdint.h>
#include <stddef.h>

struct pci_header{
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
}__attribute__((packed));

typedef struct pci_header pci_header_t;

void init_pci();

