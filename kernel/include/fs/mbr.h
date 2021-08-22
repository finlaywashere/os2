#ifndef MBR_H
#define MBR_H

#include <stdint.h>
#include <stddef.h>

#define ATTR_BOOTABLE 0b10000000

struct mbr_entry{
	uint8_t attributes;
	uint32_t chs_start : 24;
	uint8_t type;
	uint32_t chs_end : 24;
	uint32_t lba_start;
	uint32_t num_sectors;
}__attribute__((packed));
typedef struct mbr_entry mbr_entry_t;

struct mbr{
	char bootstrap[440];
	uint32_t disk_id;
	uint16_t reserved;
	mbr_entry_t entries[4];
	uint16_t signature;
}__attribute__((packed));
typedef struct mbr mbr_t;

#endif
