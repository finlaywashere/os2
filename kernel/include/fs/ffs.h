#ifndef FFS_H
#define FFS_H

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

struct chain_block{
	uint64_t next_sector[32];
}__attribute__((packed));
typedef struct chain_block chain_block_t;

struct ffs{
	parameter_block_t parameters;
	chain_block_t* chain_blocks;
};
typedef struct ffs ffs_t;

void init_ffs();
void ffs_write_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer);
void ffs_read_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer);
void ffs_read_dir(fs_node_t* dir, fs_node_t* buffer);
void ffs_write_dir(fs_node_t* file, uint64_t offset, uint64_t length, fs_node_t* buffer);
uint64_t ffs_dir_entries(fs_node_t* file);

#endif
