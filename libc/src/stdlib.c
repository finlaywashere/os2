#include <stdlib.h>

void abort(){
	//TODO: Implement abort
}
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
void* malloc(size_t size){
	//TODO: Implement malloc
}
void exit(int code){
	termination_function();
	//TODO: Implement exit with error code
}
