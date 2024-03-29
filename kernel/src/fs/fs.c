#include <fs/fs.h>
#include <utils.h>
#include <fs/devfs.h>
#include <arch/x86_64/driver/disk/disk.h>

fs_node_t* true_root;
fs_node_t** mount_points;

void init_filesystems(){
	init_fat();
	uint8_t disk_count = get_disk_count()+1; // Add 1 to support 1 extra disk
	mount_points = (fs_node_t**) kmalloc_p(sizeof(fs_node_t*)*disk_count);
	for(int i = 0; i < disk_count; i++){
		mount_points[i] = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*1000); // Assume 1000 inodes because disk_size doesnt work rn
	}
}
void set_true_root(fs_node_t* root){
	true_root = root;
}
fs_node_t* get_true_root(){
	return true_root;
}
uint8_t mkdir(char* name, uint16_t flags, fs_node_t* dir){
	return create_file(name,flags,0,dir);
}
uint8_t create_file(char* name, uint16_t flags, uint16_t type, fs_node_t* dir){
	if(dir->type != 0)
		return 1;
	return dir->create_file(dir,name,flags,type);
}
uint8_t mount(fs_node_t* folder, fs_node_t* mount){
	uint8_t disk = folder->inode >> 56;
	memcpy(&mount_points[disk][(uint32_t) folder->inode],mount,sizeof(fs_node_t*));
}
fs_node_t* get_mount(fs_node_t* folder){
	uint8_t disk = folder->inode >> 56;
	return &mount_points[disk][(uint32_t) folder->inode];
}
void get_file(char* name, fs_node_t* dst_buffer, fs_node_t* parent){
	fs_node_t* root_directory;
	if(is_kernel() == 0){
		root_directory = get_process()->root_directory;
	}else{
		root_directory = true_root;
	}
	if(name[0] == '/' && name[1] == 0x0){
		memcpy((uint8_t*) dst_buffer,(uint8_t*) root_directory,sizeof(fs_node_t));
		return;
	}
	fs_node_t* curr = parent;
	uint64_t len = strlen(name);
	uint64_t last_slash = 0;
	for(uint64_t i = 0; i < len; i++){
		fs_node_t* m_point = get_mount(curr);
		if(m_point->exists != 0x0){
			curr = m_point;
		}
		if(i == 0 && name[i] == '/'){
			last_slash = 1;
			curr = root_directory;
			continue;
		}
		if(name[i] == '/' && i != len-1){
			char segment[21];
			for(uint64_t j = last_slash; j <= last_slash+20; j++){
				if(j < i)
					segment[j-last_slash] = name[j];
				else
					segment[j-last_slash] = 0;
			}
			uint64_t dir_length = curr->dir_entries(curr);
			fs_node_t* buffer = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*dir_length);
			curr->read_dir(curr,buffer);
			if(strcmp(segment, ".")){
				last_slash = i + 1;
				continue;
            }
			if(strcmp(segment, "..")){
				if(curr->inode != root_directory->inode){
					curr = curr->parent;
				}
				last_slash = i + 1;
				continue;
			}
			if(curr == root_directory && strcmp(segment, "dev")){
				curr = get_devfs();
				last_slash = i + 1;
				continue;
			}
			for(uint64_t j = 0; j < dir_length; j++){
				if(memcmp((uint8_t*) segment, (uint8_t*) buffer[j].name, 20)){
					curr = &buffer[j];
					if(curr->type != 0){
						memcpy((uint8_t*) dst_buffer,(uint8_t*) curr,sizeof(fs_node_t));
						return;
					}
				}
			}
			last_slash = i + 1;
		}
		if(i == len-1){
			char segment[21];
			int last_slash2 = 0;
			int last_char = 0;
			for(uint64_t j = last_slash; j <= last_slash+20; j++){
				if(j <= i){
					segment[j-last_slash] = name[j];
					if(name[j] != '/')
						last_char = j-last_slash;
					else
						last_slash2 = j-last_slash;
				}
				else
					segment[j-last_slash] = 0;
			}
			if(segment[0] == 0x0){
				memcpy((uint8_t*) dst_buffer,(uint8_t*) curr,sizeof(fs_node_t));
				return;
			}
			if(last_slash2 > last_char){
				// Segment ends with slash
				segment[last_slash2] = 0x0;
			}
           	uint64_t dir_length = curr->dir_entries(curr);
			fs_node_t* buffer = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*dir_length);
			curr->read_dir(curr,buffer);
			if(strcmp(segment, ".")){
				memcpy((uint8_t*) dst_buffer,(uint8_t*) curr,sizeof(fs_node_t));
				kfree_p(buffer,sizeof(fs_node_t)*dir_length);
				return;
			}
			if(strcmp(segment, "..")){
				fs_node_t* parent;
				if(curr->inode != root_directory->inode){
					parent = curr;
				}else{
					parent = curr->parent;
				}
				memcpy((uint8_t*) dst_buffer,(uint8_t*) parent,sizeof(fs_node_t));
                kfree_p(buffer,sizeof(fs_node_t)*dir_length);
                return;
			}
			if(strcmp(segment, "dev")){
				memcpy((uint8_t*) dst_buffer,(uint8_t*) get_devfs(),sizeof(fs_node_t));
				kfree_p(buffer,sizeof(fs_node_t)*dir_length);
				return;
			}
			for(uint64_t j = 0; j < dir_length; j++){
				if(memcmp((uint8_t*) segment, (uint8_t*) buffer[j].name, 20)){
					curr = &buffer[j];
					memcpy((uint8_t*) dst_buffer,(uint8_t*) curr,sizeof(fs_node_t));
					kfree_p(buffer,sizeof(fs_node_t)*dir_length);
					return;
				}
			}
		}
	}
	if(len == 1 && name[0] == '/') // Unless asking for root then the file isn't found
		dst_buffer->exists = 0;
	return;
}
