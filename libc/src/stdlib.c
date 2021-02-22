#include <stdlib.h>

void mmap(void* addr,size_t len,int prot,int flags,int fd,int offset){
	syscall(5,(uint64_t) addr, len, ((uint64_t) prot) | (((uint64_t) flags) << 32),fd,offset);
}
void abort(){
	//TODO: Implement abort
}
void (*termination_function)(void);
int atexit(void (*func)(void)){
	termination_function = func;
}
int atoi(const char* str){
	//TODO: Implement atoi
}
void free(void* addr){
	//TODO: Implement free
}
char* getenv(const char* name){
	//TODO: Implement getenv
}
uint64_t curr_addr = 0x10000000;
uint64_t curr_alloc_addr = 0x0;

void* malloc(size_t size){
	//TODO: Implement better malloc
	if(curr_alloc_addr < curr_addr){
		curr_alloc_addr = curr_addr;
		mmap((void*) curr_alloc_addr, size, PROT_READ|PROT_WRITE, MAP_ANONYMOUS,0,0);
		curr_alloc_addr += 0x200000;
	}
	curr_addr += size;
	return (void*) curr_addr-size;
}
void exit(int code){
	termination_function();
	//TODO: Implement exit with error code
}
