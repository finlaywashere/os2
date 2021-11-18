#include <stdio.h>
#include <stdlib.h>

#define COUNT 10

int main(int argc, char** argv){
	// Directory is hard coded as . for now
	FILE* dir;
	if(argc > 1){
		dir = fopen(argv[1], "r");
	}else{
		dir = fopen(".", "r");
	}
	if(dir->id == 0){
		printf("File not found!\n");
		exit(1);
	}
	file_info_t* buffer = (file_info_t*) malloc(sizeof(file_info_t*)*COUNT);
	int actual_count = readdir(dir,COUNT,buffer);
	for(int i = 0; i < actual_count; i++){
		printf(buffer[i].name);
		printf("\n");
	}
	return 0;
}
