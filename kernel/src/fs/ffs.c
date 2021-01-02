#include <fs/ffs.h>

parameter_block_t* parameter_blocks;

uint8_t root_disk;

void ffs_read_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer){
	
}
void ffs_write_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer){
	
}

void init_ffs(){
	root_disk = 0xff;
	uint8_t disk_count = get_disk_count();
	parameter_blocks = (parameter_block_t*) kmalloc_p(sizeof(parameter_block_t)*disk_count);
	for(uint8_t i = 0; i < disk_count; i++){
		uint8_t* buffer = (uint8_t*) kmalloc_p(512);
		read_disk(i,3,1,buffer);
		parameter_block_t* pb = (parameter_block_t*) buffer;
		if(memcmp((uint8_t*) pb->signature, (uint8_t*) "FFS2",4)){
			log_warn("Found working filesystem!\n");
			parameter_blocks[i] = *pb;
			if(pb->flags & 1)
				root_disk = i;
		}else{
			kfree_p(buffer,512);
			continue;
		}
	}
	if(root_disk == 0xff)
		panic("Error: No root drive found!");
	ffs_entry_t entry = parameter_blocks[root_disk].root_directory;
	fs_node_t *root = (fs_node_t*) kmalloc_p(sizeof(fs_node_t));
	memcpy((uint8_t*) &entry.name, (uint8_t*) root->name, 20);
	root->flags = entry.permissions;
	root->inode = entry.inode;
	root->creation_time = entry.creation_date;
	root->modification_time = entry.modification_date;
	root->read_file = &ffs_read_file;
	root->write_file = &ffs_write_file;
	set_root_directory(root);
}
fs_node_t* ffs_get_file(char* name);

