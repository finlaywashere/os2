#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv){
	if(argc == 1){
		printf("Usage: cat.o <file>\n");
		exit(1);
	}
	// File is hard coded as test.txt for now
	FILE *file = fopen(argv[1],"r");
	if(file->id == 0){
		printf("File not found!\n");
		exit(1);
	}
	fseek(file,0,SEEK_END);
	uint64_t length = ftell(file);
	fseek(file,0,SEEK_SET);
	// Now the file is open to the beginning and we have the length
	char* buffer = (char*) malloc(length);
	fread(buffer,1,length,file);
	fclose(file);
	puts("Contents of /test.txt");
	printf(buffer);
	puts("\n");
}
