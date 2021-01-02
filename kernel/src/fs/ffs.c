#include <fs/ffs.h>

ffs_t* ffs;

uint8_t root_disk;

void ffs_read_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer){
	uint64_t first_sector = file->inode & 0x00FFFFFFFFFFFFFF;
	uint64_t offset_sector_index = offset/512;
	uint64_t length_sectors = length/512 + (length % 512 > 0 ? 1 : 0);
	uint64_t last_sector_index = offset_sector_index+length_sectors;
	uint64_t sector = first_sector;
	uint8_t fs_index = (uint8_t) (file->inode >> 56); // Also the disk number
	for(int i = 0; i < last_sector_index; i++){
		if(i >= offset_sector_index){
			read_disk(fs_index, sector, 1, (uint8_t*) (((uint64_t)buffer)+512*i));
		}
		
		uint64_t cb_index = sector / 32;
		uint8_t cb_pos = sector % 32;
		
		sector = ffs[fs_index].chain_blocks[cb_index].next_sector[cb_pos];
	}
}

void ffs_read_dir(fs_node_t* dir, fs_node_t* buffer){
	uint64_t entry_count = (dir->length/sizeof(ffs_entry_t));
	ffs_entry_t* entries = (ffs_entry_t*) kmalloc_p(sizeof(ffs_entry_t)*entry_count);
	ffs_read_file(dir,0,dir->length, (uint8_t*) entries);
	for(uint64_t i = 0; i < entry_count; i++){
		buffer[i].flags = entries[i].permissions;
		memcpy(entries[i].name,buffer[i].name,20); // Copy name
		buffer[i].type = entries[i].type;
		buffer[i].inode = (entries[i].inode & 0x00FFFFFFFFFFFFFF) | (((uint8_t) dir->inode) << 56); // First 8 bits of inode is the fs identifier (unique to this instance of the kernel)
		buffer[i].creation_time = entries[i].creation_date;
		buffer[i].modification_time = entries[i].modification_date;
		buffer[i].length = entries[i].length;
		buffer[i].read_file = &ffs_read_file;
		buffer[i].write_file = &ffs_write_file;
	}
	kfree_p(entries,sizeof(ffs_entry_t)*entry_count);
}

void ffs_write_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer){
	
}

void init_ffs(){
	root_disk = 0xff;
	uint8_t disk_count = get_disk_count();
	ffs = (ffs_t*) kmalloc_p(sizeof(ffs_t)*disk_count);
	for(uint8_t i = 0; i < disk_count; i++){
		uint8_t* buffer = (uint8_t*) kmalloc_p(512);
		read_disk(i,3,1,buffer);
		parameter_block_t* pb = (parameter_block_t*) buffer;
		if(memcmp((uint8_t*) pb->signature, (uint8_t*) "FFS2",4)){
			log_warn("Found working filesystem!\n");
			ffs[i].parameters = *pb;
			uint64_t num_chain_sectors = (pb->first_data_sector)-(pb->chain_start_sector);
			chain_block_t* chain_blocks = (chain_block_t*) kmalloc_p(sizeof(chain_block_t)*num_chain_sectors);
			read_disk(i,pb->chain_start_sector,num_chain_sectors,(uint8_t*)chain_blocks);
			ffs[i].chain_blocks = chain_blocks;
			if(pb->flags & 1)
				root_disk = i;
		}else{
			kfree_p(buffer,512);
			continue;
		}
	}
	if(root_disk == 0xff)
		panic("Error: No root drive found!");
	ffs_entry_t entry = ffs[root_disk].parameters.root_directory;
	fs_node_t *root = (fs_node_t*) kmalloc_p(sizeof(fs_node_t));
	memcpy((uint8_t*) &entry.name, (uint8_t*) root->name, 20);
	root->flags = entry.permissions;
	root->inode = ((uint64_t)entry.start_sector) | ((uint64_t)root_disk << 56);
	root->creation_time = entry.creation_date;
	root->modification_time = entry.modification_date;
	root->read_file = &ffs_read_file;
	root->write_file = &ffs_write_file;
	root->length = entry.length;
	set_root_directory(root);
}
fs_node_t* ffs_get_file(char* name);

