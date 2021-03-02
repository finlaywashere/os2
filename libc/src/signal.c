#include <signal.h>

int kill(pid_t pid, int sig){
	syscall(20,sig,0,0,0,0);
}
