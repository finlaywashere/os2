#include <unistd.h>

int execv(const char* file, char* const argv[]){
	syscall(4,file,0,0,0,0); // Exec syscall
	return -1; // Should never reach here
}
int execve(const char* file, char* const argv[], char* const envp[]){
	
}
int execvp(const char* file, char* const argv[]){
	
}
pid_t fork(){
	return (pid_t) syscall(3,0,0,0,0,0); // Fork syscall, returns child pid (or 0 if child) in rax
}
