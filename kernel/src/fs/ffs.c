#include <fs/ffs.h>

ffs_t* ffs;

uint8_t root_disk;

void ffs_update_file(fs_node_t* file){
	fs_node_t* parent = file->parent;
	uint64_t num_entries = parent->dir_entries(parent);
	fs_node_t* buffer = (fs_node_t*) kmalloc_p(sizeof(fs_node_t) * num_entries);
	parent->read_dir(parent,buffer);
	for(uint64_t i = 0; i < num_entries; i++){
		if(strcmp(&file->name,&buffer[i].name)){
			// Found entry
			uint64_t offset = i * sizeof(fs_node_t);
			parent->write_dir(parent,offset,1,file);
		}
	}
}

void write_chain_entries(uint8_t fs_index){
	write_disk(fs_index, ffs[fs_index].parameters.chain_start_sector, ffs[fs_index].parameters.first_data_sector-ffs[fs_index].parameters.chain_start_sector, ffs[fs_index].chain_blocks);
}

uint64_t ffs_dir_entries(fs_node_t* file){
	if(file->type != 0)
		return 0;
	return file->length/sizeof(ffs_entry_t);
}
uint8_t ffs_create_file(fs_node_t* dir, char* name, uint16_t flags, uint16_t type){
	uint8_t fs_index = (uint8_t) (dir->inode >> 56); // Grab disk number from file inode
	uint64_t sector = 0;
	for(uint64_t j = 0; j < ffs[fs_index].parameters.first_data_sector-ffs[fs_index].parameters.chain_start_sector; j++){
		for(int k = 0; k < 32; k++){
			if(ffs[fs_index].chain_blocks[j].next_sector[k] == 0){
				// Found free sector
				ffs[fs_index].chain_blocks[j].next_sector[k] = 0xffffffffffffffff;
				sector = j*32+k;
				break;
			}
		}
	}
	if(sector == 0){
		return 1;
	}
	write_chain_entries(fs_index);
	fs_node_t* entry = (fs_node_t*) kmalloc_p(sizeof(fs_node_t));
	entry->flags = flags;
	entry->type = type;
	uint64_t len = strlen(name);
	memcpy(name,entry->name,len);
	entry->inode = (((uint64_t)((uint8_t)(dir->inode >> 56))) << 56) | sector; // Set disk number and start sector as inode
	uint64_t entries = dir->dir_entries(dir);
	dir->write_dir(dir,entries*sizeof(ffs_entry_t),sizeof(ffs_entry_t), entry);
}
void ffs_read_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer){
	uint64_t first_sector = file->inode & 0x00FFFFFFFFFFFFFF;
	uint64_t offset_sector_index = offset/512;
	uint64_t length_sectors = length/512 + (length % 512 > 0 ? 1 : 0);
	uint64_t last_sector_index = offset_sector_index+length_sectors;
	uint64_t sector = first_sector;
	uint8_t fs_index = (uint8_t) (file->inode >> 56); // Also the disk number
	uint64_t tmp_length = length+(length%512 > 0 ? 512-length%512 : 0);
	uint8_t* tmp_buffer = (uint8_t*) kmalloc_p(tmp_length);
	for(int i = 0; i < last_sector_index; i++){
		if(i >= offset_sector_index){
			read_disk(fs_index, sector, 1, (uint8_t*) (((uint64_t)tmp_buffer)+512*i));
		}
		
		uint64_t cb_index = sector / 32;
		uint8_t cb_pos = sector % 32;
		
		sector = ffs[fs_index].chain_blocks[cb_index].next_sector[cb_pos];
	}
	memcpy((uint8_t*)(((uint64_t)tmp_buffer)+offset%512),buffer,length);
	kfree_p(tmp_buffer,tmp_length);
}

void ffs_read_dir(fs_node_t* dir, fs_node_t* buffer){
	uint64_t entry_count = (dir->length/sizeof(ffs_entry_t));
	ffs_entry_t* entries = (ffs_entry_t*) kmalloc_p(sizeof(ffs_entry_t)*entry_count);
	ffs_read_file(dir,0,dir->length, (uint8_t*) entries);
	for(uint64_t i = 0; i < entry_count; i++){
		buffer[i].flags = entries[i].permissions;
		memcpy(entries[i].name,buffer[i].name,20); // Copy name
		buffer[i].type = entries[i].type;
		buffer[i].inode = (((uint64_t) (dir->inode>>56)) << 56) | entries[i].start_sector; // First 8 bits of inode is the fs identifier (unique to this instance of the kernel)
		buffer[i].creation_time = entries[i].creation_date;
		buffer[i].modification_time = entries[i].modification_date;
		buffer[i].length = entries[i].length;
		buffer[i].read_file = &ffs_read_file;
		buffer[i].write_file = &ffs_write_file;
		buffer[i].read_dir = &ffs_read_dir;
		buffer[i].dir_entries = &ffs_dir_entries;
		buffer[i].write_dir = &ffs_write_dir;
		buffer[i].create_file = &ffs_create_file;
		buffer[i].parent = dir;
	}
	kfree_p(entries,sizeof(ffs_entry_t)*entry_count);
}

void ffs_write_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer){
	uint64_t file_length = file->length/512 + (file->length % 512 > 0 ? 1 : 0);
	uint8_t* new_buffer = (uint8_t*) kmalloc_p(file_length*512);
	memcpy(buffer,new_buffer,length);
	signed long size_diff = (file->length-offset-length) * -1;
	if(size_diff < 0)
		size_diff = 0;
	if(file->length < offset+length){
		file->length == offset+length;
		ffs_update_file(file);
	}
	uint64_t new_blocks = size_diff/512+(size_diff%512>0?1:0);
	uint64_t offset_sectors = offset/512;
	uint64_t *blocks = (uint64_t*) kmalloc_p(sizeof(uint64_t)*(new_blocks+1));
	uint64_t first_sector = file->inode & 0x00FFFFFFFFFFFFFF;
	uint8_t fs_index = (uint8_t) (file->inode >> 56); // Grab disk number from file inode
	uint64_t num_chain_sectors = (ffs[fs_index].parameters.first_data_sector)-(ffs[fs_index].parameters.chain_start_sector);
	uint64_t last_sector = 0xffffffffffffffff;
	for(uint64_t i = new_blocks; i > 0; i--){
		int found = 0;
		for(uint64_t j = 0; j < num_chain_sectors; j++){
			if(found)
				break;
			for(int k = 0; k < 32; k++){
				if(ffs[fs_index].chain_blocks[j].next_sector[k] == 0){
					// Found free sector
					ffs[fs_index].chain_blocks[j].next_sector[k] = last_sector;
					blocks[i] = j*32+k;
					last_sector = blocks[i];
					found = 1;
					break;
				}
			}
		}
		if(found == 0){
			panic("Disk full!");
		}
	}
	uint64_t sector = first_sector;
	
	for(uint64_t i = 0; i < file_length; i++){
		uint64_t next_sector = ffs[fs_index].chain_blocks[sector/32].next_sector[sector%32];
		if(next_sector == 0xffffffffffffffff)
			break;
		sector = next_sector;
	}
	blocks[0] = sector;
	
	if(new_blocks > 0){
		ffs[fs_index].chain_blocks[blocks[0]/32].next_sector[blocks[0]%32] = blocks[1]; // Link together parts of the chain
		write_chain_entries(fs_index);
	}
	
	for(uint64_t i = 0; i < new_blocks+1; i++){
		if(i > offset_sectors || (i == offset_sectors && offset == 0)){
			write_disk(fs_index, blocks[i], 1, (uint8_t*)(((uint64_t)new_buffer)+512*i));
		}else if(i == offset_sectors && offset % 512 > 0){
			uint8_t* tmp_buffer = (uint8_t*) kmalloc_p(512); // Allocate 512 byte temporary buffer
			
			read_disk(fs_index, blocks[i], 1, tmp_buffer);
			memcpy(new_buffer,(uint8_t*) (((uint64_t)tmp_buffer)+offset%512),512-offset%512);
			write_disk(fs_index, blocks[i], 1, tmp_buffer);
			
			kfree_p((uint64_t) tmp_buffer, 512);
		}
	}
}
void ffs_write_dir(fs_node_t* file, uint64_t offset, uint64_t count, fs_node_t* buffer){
	uint64_t num_entries = count;
	uint64_t actual_size = num_entries*sizeof(ffs_entry_t);
	ffs_entry_t* entries = (ffs_entry_t*) kmalloc_p(actual_size);
	for(uint64_t i = 0; i < num_entries; i++){
		memcpy(&buffer[i].name,&entries[i].name,20); // Copy name
		entries[i].length = buffer[i].length;
		entries[i].start_sector = buffer[i].inode & 0x00FFFFFFFFFFFFFF;
		entries[i].type = buffer[i].type;
	}
	file->write_file(file,offset,actual_size,(uint8_t*) entries);
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
	root->read_dir = &ffs_read_dir;
	root->dir_entries = &ffs_dir_entries;
	root->write_dir = &ffs_write_dir;
	root->length = entry.length;
	root->create_file = &ffs_create_file;
	set_root_directory(root);
}
