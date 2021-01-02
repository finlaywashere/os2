#pragma once

#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <mem/vmm.h>
#include <utils.h>
#include <log.h>

#define FFS_DIRECTORY 0
#define FFS_FILE 1
#define FFS_SYMLINK 2

struct ffs_entry{
	char name[20];
	uint16_t type;
	uint16_t permissions;
	uint64_t inode;
	uint64_t creation_date;
	uint64_t modification_date;
	uint64_t start_sector;
	uint64_t length;
}__attribute__((packed));
typedef struct ffs_entry ffs_entry_t;

struct parameter_block{
	char signature[4];
	uint32_t flags;
	uint64_t sectors;
	uint64_t chain_start_sector;
	uint64_t first_data_sector;
	char reserved[416];
	ffs_entry_t root_directory;
}__attribute__((packed));
typedef struct parameter_block parameter_block_t;

void init_ffs();
