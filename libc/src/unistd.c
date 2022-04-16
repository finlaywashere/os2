#include <unistd.h>
#include <syscall.h>

int execv(const char* file, char* const argv[]){
	syscall(4,file,argv,0,0,0); // Exec syscall
	return -1; // Should never reach here
}
int execve(const char* file, char* const argv[], char* const envp[]){
	
}
int execvp(const char* file, char* const argv[]){
	
}
pid_t fork(){
	return (pid_t) syscall(3,0,0,0,0,0); // Fork syscall, returns child pid (or 0 if child) in rax
}
uid_t getuid(){
	return syscall(21,0,0,0,0,0);
}
gid_t getgid(){
	return syscall(22,0,0,0,0,0);
}
pid_t getpid(){
	return syscall(23,0,0,0,0,0);
}
int setuid(uid_t uid){
	syscall(24,uid,0,0,0,0);
	return 0;
}
int setgid(gid_t gid){
	syscall(25,gid,0,0,0,0);
	return 0;
}
int chdir(const char* dir){
	return syscall(27,dir,0,0,0,0);
}
int chroot(const char* path){
	return syscall(26,path,0,0,0,0);
}
