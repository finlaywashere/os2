#include <fs/devfs.h>
#include <mem/vmm.h>

fs_node_t* devfs_root;
fs_node_t* entries;

uint64_t dev_dir_entries(fs_node_t* file){
	return MAX_DEVFS_ENTRIES;
}
uint8_t dev_read_dir(fs_node_t* file, fs_node_t* buffer){
	memcpy(buffer,entries,sizeof(fs_node_t)*MAX_DEVFS_ENTRIES);
	return 0;
}

void init_devfs(){
	devfs_root = (fs_node_t*) kmalloc_p(sizeof(fs_node_t));
	entries = (fs_node_t*) kmalloc_p(sizeof(fs_node_t) * MAX_DEVFS_ENTRIES);
	devfs_root->name[0] = '/';
	devfs_root->type = 1;
	devfs_root->dir_entries = &dev_dir_entries;
	devfs_root->read_dir = &dev_read_dir;
	devfs_root->exists = 1;
}
fs_node_t* get_devfs(){
	return devfs_root;
}
void register_folder(fs_node_t* dir){
	for(int i = 0; i < MAX_DEVFS_ENTRIES; i++){
		if(entries[i].exists == 0){
			memcpy(&entries[i],dir,sizeof(fs_node_t));
			return;
		}
	}
	panic("Out of devfs space!");
}
