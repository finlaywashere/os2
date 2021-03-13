#include <stdio.h>
#include <stdlib.h>
#include <sys/utsname.h>

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
	fseek(desc,5,SEEK_SET);
	char* test2 = "hel";
	fwrite(test2,1,3,desc);
	struct utsname *name = (struct utsname*) malloc(sizeof(struct utsname));
	uname(name);
	printf(name->sysname);
	while(1){
		char buffer[1];
		fread(&buffer,1,1,stdin);
		if(buffer[0] != 0x0)
			fwrite(&buffer,1,1,stdout);
	}
	while(1);
}
