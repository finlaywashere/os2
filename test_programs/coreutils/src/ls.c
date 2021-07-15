#include <stdio.h>
#include <stdlib.h>

#define COUNT 10

int main(int argc, char** argv){
	// Directory is hard coded as / for now
	FILE* dir = fopen("/", "r");
	file_info_t* buffer = (file_info_t*) malloc(sizeof(file_info_t*)*COUNT);
	int actual_count = readdir(dir,COUNT,buffer);
	printf("Files in root directory:\n\n");
	for(int i = 0; i < actual_count; i++){
		printf(buffer[i].name);
		printf("\n");
	}
	return 0;
}
