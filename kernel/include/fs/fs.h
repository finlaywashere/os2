#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>

#include <mem/vmm.h>

struct fs_node{
	uint16_t flags; // File flags
	char name[20];
	uint16_t type; // File type, 0 = directory, 1 = file, 2 = symlink, etc
	uint64_t inode; // File unique identifier
	uint64_t creation_time; // Ms since epoch
	uint64_t modification_time; // Ms since epoch
	uint64_t length; // In bytes
	struct fs_node* parent;
	uint8_t (*read_file) (struct fs_node *file, uint64_t offset, uint64_t count, uint8_t* buffer);
	uint8_t (*read_dir) (struct fs_node *file, struct fs_node* buffer);
	uint64_t (*dir_entries) (struct fs_node *file);
	uint8_t (*write_file) (struct fs_node *file, uint64_t offset, uint64_t count, uint8_t* buffer);
	uint8_t (*write_dir) (struct fs_node *file, uint64_t offset, uint64_t count, struct fs_node* buffer);
};
typedef struct fs_node fs_node_t;

#include <fs/ffs.h>

void init_filesystems();
void get_file(char* name, fs_node_t* dst_buffer);
void set_root_directory(fs_node_t* root);
void create_file(char* name,fs_node_t* file);
fs_node_t* get_root_directory();
#endif

