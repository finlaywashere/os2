#include <fs/fs.h>
#include <utils.h>

fs_node_t* root_directory;

void init_filesystems(){
	init_fat();
}
uint8_t mkdir(char* name, uint16_t flags, fs_node_t* dir){
	return create_file(name,flags,0,dir);
}
uint8_t create_file(char* name, uint16_t flags, uint16_t type, fs_node_t* dir){
	if(dir->type != 0)
		return 1;
	return dir->create_file(dir,name,flags,type);
}
void get_file(char* name, fs_node_t* dst_buffer, fs_node_t* parent){
	if(name[0] == '/' && name[1] == 0x0){
		memcpy((uint8_t*) root_directory, (uint8_t*) dst_buffer,sizeof(fs_node_t));
		return;
	}
	fs_node_t* curr = parent;
	uint64_t len = strlen(name);
	uint64_t last_slash = 0;
	for(uint64_t i = 0; i < len; i++){
		if(i == 0 && name[i] == '/'){
			last_slash = 1;
			curr = root_directory;
			continue;
		}
		if(name[i] == '/'){
			char segment[21];
			for(uint64_t j = last_slash; j <= 20; j++){
				if(j < i)
					segment[j-last_slash] = name[j];
				else
					segment[j-last_slash] = 0;
			}
			uint64_t dir_length = curr->dir_entries(curr);
			fs_node_t* buffer = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*dir_length);
			curr->read_dir(curr,buffer);
			if(strcmp(segment, ".")){
                continue;
            }
			for(uint64_t j = 0; j < dir_length; j++){
				if(memcmp((uint8_t*) segment, (uint8_t*) buffer[j].name, 20)){
					curr = &buffer[j];
					if(curr->type != 0){
						memcpy((uint8_t*) curr, (uint8_t*) dst_buffer,sizeof(fs_node_t));
						return;
					}
				}
			}
		}
		if(i == len-1){
			char segment[21];
			for(uint64_t j = last_slash; j <= 20; j++){
				if(j <= i)
					segment[j-last_slash] = name[j];
				else
					segment[j-last_slash] = 0;
			}
           	uint64_t dir_length = curr->dir_entries(curr);
			fs_node_t* buffer = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*dir_length);
			curr->read_dir(curr,buffer);
			if(strcmp(segment, ".")){
				memcpy((uint8_t*) curr, (uint8_t*) dst_buffer,sizeof(fs_node_t));
				kfree_p(buffer,sizeof(fs_node_t)*dir_length);
				return;
			}
			for(uint64_t j = 0; j < dir_length; j++){
				if(memcmp((uint8_t*) segment, (uint8_t*) buffer[j].name, 20)){
					curr = &buffer[j];
					memcpy((uint8_t*) curr, (uint8_t*) dst_buffer,sizeof(fs_node_t));
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
void set_root_directory(fs_node_t* root){
	root_directory = root;
}
fs_node_t* get_root_directory(){
	return root_directory;
}
