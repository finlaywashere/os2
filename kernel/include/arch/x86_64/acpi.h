#include <stdint.h>
#include <stddef.h>
#include <utils.h>

struct rsdp{
	char sig[8];
	uint8_t checksum;
	char oem[6];
	uint8_t revision;
	uint32_t rsdt;
}__attribute__((packed));

typedef struct rsdp rsdp_t;

void init_acpi();

