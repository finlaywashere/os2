#include <security/user.h>
#include <process/process.h>

uint64_t get_uid(){
	process_t* process = get_process();
	return process->uid;
}
uint64_t get_gid(){
	process_t* process = get_process();
	return process->gid;
}
int is_superuser(){
	return get_uid() == 0;
}
int set_uid(uint64_t uid){
	process_t* process = get_process();
	process->uid = uid;
}
int set_gid(uint64_t gid){
	process_t* process = get_process();
	process->gid = gid;
}
