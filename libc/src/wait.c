#include <sys/wait.h>

uint64_t waitpid(pid_t pid){
	return syscall(29,pid,0,0,0,0);
}
