#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
	// File is hard coded as test.txt for now
	FILE *file = fopen("/test.txt","r");
	fseek(file,0,SEEK_END);
	uint64_t length = ftell(file);
	fseek(file,0,SEEK_SET);
	// Now the file is open to the beginning and we have the length
	char* buffer = (char*) malloc(length);
	fread(buffer,1,length,file);
	fclose(file);
	puts(buffer);
	puts("\n");
}
