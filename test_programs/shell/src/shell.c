#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

void main(){
	while(1){
		printf("[FinlayOS]$ ");
		char* buffer = malloc(1024); // Allocate 1024 byte input buffer
		uint64_t index = 0;
		while(1){
			uint64_t count = fread(&buffer[index],1,1,stdin);
			uint64_t oldindex = index;
			index += count;
			if(buffer[oldindex] == '\n'){
				buffer[oldindex] = 0x0;
				break;
			}
			if(buffer[oldindex] > 0x10){
				fwrite(&buffer[oldindex],1,1,stdout);
			}else if(buffer[0] == 0x1){
				buffer[oldindex] = 0x0;
				buffer[oldindex-1] = 0x0;
				index--;
				fseek(stdout,-1,SEEK_CUR);
				fwrite(" ",1,1,stdout);
				fseek(stdout,-1,SEEK_CUR);
			}else if(buffer[0] == 0x4){
				fseek(stdout,1,SEEK_CUR);
				index++;
			}else if(buffer[0] == 0x5){
				fseek(stdout,-1,SEEK_CUR);
				index--;
			}
		}
		printf("\n");
		if(memcmp("cd ",buffer,3) == 0){
			// Command is to change dir
			uint64_t result = chdir(&buffer[3]);
		}
		FILE* fd = fopen(buffer,"r");
		if(fd->id == 0){
			printf("File not found!\n");
			continue;
		}
		fclose(fd);
		uint64_t pid = fork(); // Fork and execute new program
		if(pid == 0){
			execv(buffer, 0x0);
		}else{
			waitpid(pid);
		}
	}
}
