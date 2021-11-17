#include <stdio.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>

void error(uint64_t error, uint64_t rip, uint64_t information){
	printf("Received error!");
}
int main(){
	register_signal(&error,0,HANDLER_ERROR,0);
	return 1/0;
}
