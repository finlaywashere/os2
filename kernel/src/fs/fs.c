#include <fs/fs.h>

fs_node_t* root_directory;

void init_filesystems(){
	init_ffs();
}
fs_node_t* get_file(char* name){
	return root_directory;
}
void set_root_directory(fs_node_t* root){
	root_directory = root;
}
