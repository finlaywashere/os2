#include <sys/utsname.h>
int uname(struct utsname *buf){
	syscall(11,(uint64_t) buf,0,0,0,0);
}
