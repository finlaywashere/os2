#ifndef DISK_H
#define DISK_H

#include <stdint.h>
#include <stddef.h>
#include <mem/vmm.h>
#include <arch/x86_64/driver/disk/ahci.h>

struct disk{
	uint8_t flags; // 1 if drive exists, 0 otherwise. Might be used later for drive info
	char name[16];
	uint32_t type; // Disk type, 0 = IDE, etc
	uint32_t identifier; // Driver specific identifier to find which disk this represents
	uint8_t (*read_disk) (struct disk *disk, uint64_t lba, uint64_t sectors, uint8_t* buffer);
	uint8_t (*write_disk) (struct disk *disk, uint64_t lba, uint64_t sectors, uint8_t* buffer);
};
typedef struct disk disk_t;

uint8_t get_disk_count();
void register_disk(disk_t disk);
disk_t* get_disk(uint8_t index);
void init_disks();

uint8_t read_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer);
uint8_t write_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer);

#endif
