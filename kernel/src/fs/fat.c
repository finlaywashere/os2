#include <fs/fat.h>
#include <fs/mbr.h>

uint16_t fat_root_disk;
uint32_t partition_sector;
uint64_t fat_partition_start;
fat_bpb_t* fat_bpb;

uint32_t __attribute__((noinline)) cluster_to_sector(uint32_t cluster){
	uint64_t first_sector = fat_bpb->bpb.reserved_sectors + (fat_bpb->bpb.num_fats * fat_bpb->ebpb.sectors_per_fat);
	return ((cluster - 2) * fat_bpb->bpb.sectors_per_cluster) + first_sector;
}
uint32_t get_next_cluster(uint32_t cluster){
	uint32_t fat[fat_bpb->bpb.bytes_per_sector];
	uint32_t fat_offset = cluster * 4;
	uint32_t fat_sector = partition_sector + fat_bpb->bpb.reserved_sectors + (fat_offset / fat_bpb->bpb.bytes_per_sector);
	uint32_t ent_offset = fat_offset % fat_bpb->bpb.bytes_per_sector;

	read_disk(fat_root_disk >> 8,fat_sector,1,fat);

	uint32_t value = fat[ent_offset] & 0x0FFFFFFF;
	return value;
}
uint64_t __attribute__ ((noinline)) num_clusters(uint32_t first_cluster){
	uint32_t first_sector = cluster_to_sector(first_cluster);
	uint32_t sectors_per_cluster = fat_bpb->bpb.sectors_per_cluster;
	uint32_t bytes_per_sector = fat_bpb->bpb.bytes_per_sector;
	uint64_t cluster_count = 0;
	uint32_t cluster = first_cluster;
	while(1){
		uint32_t tmp = get_next_cluster(cluster);
		if(tmp >= 0x0FFFFFF7)
			break;
		cluster = tmp;
		cluster_count++;
	}
	return cluster_count;
};
void get_clusters(uint32_t first_cluster, uint32_t* clusters){
	uint32_t first_sector = cluster_to_sector(first_cluster);
	uint32_t sectors_per_cluster = fat_bpb->bpb.sectors_per_cluster;
	uint32_t bytes_per_sector = fat_bpb->bpb.bytes_per_sector;
	uint64_t cluster_count = 1;
	uint32_t cluster = first_cluster;
	while(1){
		clusters[cluster_count-1] = cluster;
		uint32_t tmp = get_next_cluster(cluster);
		if(tmp >= 0x0FFFFFF7)
			break;
		cluster = tmp;
		cluster_count++;
	}
}
uint8_t fat_read_file(fs_node_t* file, uint64_t offset, uint64_t count, uint8_t* buffer){
	uint32_t first_cluster = (uint32_t) file->inode;
	uint64_t cluster_count = num_clusters(first_cluster);
	uint32_t clusters[cluster_count];
	get_clusters(first_cluster,&clusters);
	uint32_t sectors_per_cluster = fat_bpb->bpb.sectors_per_cluster;
	uint8_t* tmp_buffer = (uint8_t*) kmalloc_p(sectors_per_cluster*fat_bpb->bpb.bytes_per_sector);
	uint64_t start_byte = offset % (sectors_per_cluster*fat_bpb->bpb.bytes_per_sector);
	uint64_t sector_offset = fat_partition_start;
	
	uint64_t bytes_per_cluster = fat_bpb->bpb.sectors_per_cluster * fat_bpb->bpb.bytes_per_sector;
	uint64_t cluster_offset = offset/bytes_per_cluster;
	
	for(uint64_t i = cluster_offset; i < cluster_count; i++){
		uint64_t sector = cluster_to_sector(clusters[i]) + sector_offset;
		read_disk(fat_root_disk >> 8,sector,sectors_per_cluster,tmp_buffer);
		uint64_t buffer_index = (i-cluster_offset) * sectors_per_cluster * fat_bpb->bpb.bytes_per_sector - start_byte;
		if(i == cluster_offset){
			memcpy(&tmp_buffer[start_byte],&buffer[buffer_index],sectors_per_cluster * fat_bpb->bpb.bytes_per_sector-start_byte);
		}else{
			memcpy(tmp_buffer,&buffer[buffer_index],sectors_per_cluster * fat_bpb->bpb.bytes_per_sector);
		}
	}
	kfree_p(tmp_buffer,sectors_per_cluster*fat_bpb->bpb.bytes_per_sector);
	return 0;
}
uint8_t fat_read_dir(fs_node_t* file, fs_node_t* buffer){
	uint64_t entry_count = file->length / sizeof(fat_dir_entry_t) * 2;
	fat_dir_entry_t* entries = (fat_dir_entry_t*) kmalloc_p(sizeof(fat_dir_entry_t) * entry_count);
	fat_read_file(file,0,file->length,entries);
	uint64_t index = 0;
	for(uint64_t i = 0; i < entry_count; i++){
		if(entries[i].name[0] == 0x0 || entries[i].attributes == 0xF) continue;
		int pos = 0;
		for(int i2 = 0; i2 < 8; i2++){
			if(entries[i].name[i2] == ' ') break;
			buffer[index].name[pos] = entries[i].name[i2] + 32;
			pos++;
		}
		buffer[index].name[pos] = '.';
		pos++;
		for(int i2 = 0; i2 < 3; i2++){
			if(entries[i].ext[i2] == ' ') break;
			buffer[index].name[pos] = entries[i].ext[i2] + 32;
			pos++;
		}
		buffer[index].name[pos] = 0x0;
		buffer[index].type = entries[i].attributes & DIRECTORY ? 0 : 1;
		buffer[index].inode = (((uint64_t) (file->inode>>48)) << 48) | entries[i].cluster_high << 16 | entries[i].cluster_low; // First 8 bits of inode is the fs identifier (unique to this instance of the kernel)
		buffer[index].length = entries[i].size;
		buffer[index].read_file = &fat_read_file;
		buffer[index].write_file = &fat_write_file;
		buffer[index].read_dir = &fat_read_dir;
		buffer[index].dir_entries = &fat_dir_entries;
		buffer[index].write_dir = &fat_write_dir;
		buffer[index].create_file = &fat_create_file;
		buffer[index].parent = file;
		buffer[index].exists = 1;
		index++;
	}
}
uint64_t fat_dir_entries(fs_node_t* file){
	uint32_t start_cluster = (uint32_t) file->inode;
	uint64_t cluster_count = num_clusters(start_cluster);
	uint32_t clusters[cluster_count];
	get_clusters(start_cluster,clusters);
	uint64_t num_entries = cluster_count * fat_bpb->bpb.sectors_per_cluster * fat_bpb->bpb.bytes_per_sector / sizeof(fat_dir_entry_t);
	uint64_t result = 0;
	fat_dir_entry_t* buffer = (fat_dir_entry_t*) kmalloc_p(fat_bpb->bpb.sectors_per_cluster * fat_bpb->bpb.bytes_per_sector);
	uint64_t sector_offset = fat_partition_start;
	for(uint64_t i = 0; i < cluster_count; i++){
		read_disk(fat_root_disk>>8,cluster_to_sector(clusters[i]) + sector_offset,fat_bpb->bpb.sectors_per_cluster,buffer);
		for(uint64_t i2 = 0; i2 < fat_bpb->bpb.sectors_per_cluster * fat_bpb->bpb.bytes_per_sector / sizeof(fat_dir_entry_t); i2++){
			if(buffer[i2].name[0] == 0x0) break;
			if(buffer[i2].attributes & 0xF) continue;
			result++;
		}
	}
	kfree_p(buffer,fat_bpb->bpb.sectors_per_cluster * fat_bpb->bpb.bytes_per_sector);
	return result;
}
uint8_t fat_write_file(fs_node_t* file, uint64_t offset, uint64_t count, uint8_t* buffer){
	
}
uint8_t fat_write_dir(fs_node_t* file, uint64_t offset, uint64_t count, fs_node_t* buffer){
	
}
uint8_t fat_create_file(fs_node_t* dir, char* name, uint16_t flags, uint16_t type){
	
}

void init_fat(){
	fat_root_disk = 0xffff;
	
	fat_bpb = (fat_bpb_t*) kmalloc_p(sizeof(fat_bpb_t));
	
	uint8_t disk_count = get_disk_count();
	for(uint8_t i = 0; i < disk_count; i++){
		mbr_t* buffer = (mbr_t*) kmalloc_p(512);
		read_disk(i,0,1,buffer);
		for(int p = 0; p < 4; p++){
			mbr_entry_t entry = buffer->entries[p];
			if(entry.attributes & ATTR_BOOTABLE){
				fat_root_disk = (i << 8) | p;
				uint32_t lba_start = entry.lba_start;
				partition_sector = lba_start;
				fat_partition_start = entry.lba_start;
				read_disk(i,lba_start,1,fat_bpb);
				break;
			}
		}
	}
	if(fat_root_disk == 0xffff)
		panic("Error: No root drive found!");
	
	
	fs_node_t* root = (fs_node_t*) kmalloc_p(sizeof(fs_node_t));
	strcpy("/", &root->name);
	root->flags = 0;
	root->inode = fat_bpb->ebpb.root_cluster | ((uint64_t)fat_root_disk << 48);
	root->read_file = &fat_read_file;
	root->write_file = &fat_write_file;
	root->read_dir = &fat_read_dir;
	root->dir_entries = &fat_dir_entries;
	root->write_dir = &fat_write_dir;
	root->create_file = &fat_create_file;
	root->exists = 1;

	uint64_t entries = fat_dir_entries(root);
	root->length = entries * sizeof(fat_dir_entry_t);
	set_root_directory(root);
}
