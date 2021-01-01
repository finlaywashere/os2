#pragma once
#include <stdint.h>
#include <stddef.h>

struct fs_node{
        uint16_t flags; // File flags
        char name[16];
        uint16_t type; // File type, 0 = directory, 1 = file, 2 = symlink, etc
        uint64_t inode; // File unique identifier
	uint32_t reserved;
        uint8_t (*read_file) (struct fs_node *file, uint64_t offset, uint64_t count, uint8_t* buffer);
        uint8_t (*write_file) (struct fs_node *file, uint64_t offset, uint64_t count, uint8_t* buffer);
};
typedef struct fs_node fs_node_t;

void init_filesystems();
