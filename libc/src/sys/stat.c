#include <sys/stat.h>
#include <syscall.h>

int mkdir(const char *path, mode_t mode){
	return syscall(17,path,mode,0,0,0);
}
