#ifndef DEVFS_H
#define DEVFS_H

#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>

#define MAX_DEVFS_ENTRIES 20

void init_devfs();
void register_folder(fs_node_t* dir);
fs_node_t* get_devfs();

#endif
