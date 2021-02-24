#include <stdio.h>
#include <stdlib.h>

int main(){
	printf("Test1234");
	char* test = (char*) malloc(64);
	FILE* desc = fopen("test.txt","r");
	if(desc->id == 0){
		printf("Error: Could not find test.txt!");
		while(1);
	}
	size_t count = fread(test,1,64,desc);
	printf(test);
	char* test2 = "hello145";
	fwrite(test2,1,8,desc);
	while(1);
}
