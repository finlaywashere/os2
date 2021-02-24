#include <fs/fs.h>

fs_node_t* root_directory;

void init_filesystems(){
	init_ffs();
}
void create_file(char* name,fs_node_t* file){
	//TODO: Implement create_file
}
void get_file(char* name, fs_node_t* dst_buffer){
	fs_node_t* curr = root_directory;
	uint64_t len = strlen(name);
	uint64_t last_slash = 0;
	for(uint64_t i = 0; i < len; i++){
		if(i == 0 && name[i] == '/'){
			last_slash = 1;
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
		memcpy((uint8_t*) curr, (uint8_t*) dst_buffer,sizeof(fs_node_t));
	return;
}
void set_root_directory(fs_node_t* root){
	root_directory = root;
}
