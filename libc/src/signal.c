#include <signal.h>

int kill(pid_t pid, int sig){
	return syscall(20,sig,0,0,0,0);
}
int register_signal(void *callback, uint64_t index, uint64_t flags, uint64_t condition){
	return syscall(31,(uint64_t) callback,index,flags,condition,0);
}
