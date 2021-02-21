#ifndef ACPI_H
#define ACPI_H

#include <stdint.h>
#include <stddef.h>
#include <utils.h>
#include <log.h>

struct rsdp{
	char sig[8];
	uint8_t checksum;
	char oem[6];
	uint8_t revision;
	uint32_t rsdt;
}__attribute__((packed));

typedef struct rsdp rsdp_t;

struct acpi_header{
	char sig[4];
	uint32_t length;
	uint8_t revision;
	uint8_t checksum;
	char oem[6];
	char oemtable[8];
	uint32_t oemrevision;
	uint32_t creatorid;
	uint32_t creatorrevision;
}__attribute__((packed));

typedef struct acpi_header acpi_header_t;

void init_acpi();
int acpi_checksum(acpi_header_t* header);
acpi_header_t* get_acpi_table(char* sig);

#endif
