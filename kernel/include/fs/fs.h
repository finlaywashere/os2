#ifndef FS_H
#define FS_H

#include <stdint.h>
#include <stddef.h>

#include <mem/vmm.h>

struct fs_node{
	uint8_t exists;
	uint16_t flags; // File flags
	char name[20];
	uint16_t type; // File type, 0 = directory, 1 = file, 2 = symlink, etc
	uint64_t inode; // File unique identifier
	uint64_t creation_time; // Ms since epoch
	uint64_t modification_time; // Ms since epoch
	uint64_t length; // In bytes
	uint64_t uid;
	uint64_t gid;
	struct fs_node* parent;
	uint8_t (*read_file) (struct fs_node *file, uint64_t offset, uint64_t count, uint8_t* buffer);
	uint8_t (*read_dir) (struct fs_node *file, struct fs_node* buffer);
	uint64_t (*dir_entries) (struct fs_node *file);
	uint8_t (*write_file) (struct fs_node *file, uint64_t offset, uint64_t count, uint8_t* buffer);
	uint8_t (*write_dir) (struct fs_node *file, uint64_t offset, uint64_t count, struct fs_node* buffer);
	uint8_t (*create_file) (struct fs_node* dir, char* name, uint16_t flags, uint16_t type);
};
typedef struct fs_node fs_node_t;

struct user_fs_node{
	uint16_t flags;
	char name[20];
	uint16_t type;
	uint64_t inode;
	uint64_t creation_time;
	uint64_t modification_time;
	uint64_t length; // in bytes
	uint8_t exists;
}__attribute__((packed));
typedef struct user_fs_node user_fs_node_t;

#include <fs/ffs.h>
#include <fs/fat.h>

void init_filesystems();
void get_file(char* name, fs_node_t* dst_buffer, fs_node_t* parent);
uint8_t mkdir(char* name, uint16_t flags, fs_node_t* dir);
uint8_t create_file(char* name, uint16_t flags, uint16_t type, fs_node_t* dir);
void set_true_root(fs_node_t* root);
fs_node_t* get_true_root();

#endif

