#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

char* append(char* start, char* end){
	size_t s_len = strlen(start);
	size_t e_len = strlen(end);
	size_t len = s_len + e_len + 1;
	char* buffer = (char*) malloc(len);
	for(size_t i = 0; i < len; i++){
		if(i < s_len){
			buffer[i] = start[i];
		}else if(i < len-1){
			buffer[i] = end[i-s_len];
		}else{
			buffer[i] = 0x0;
		}
	}
	return buffer;
}

char* find_file(char* name){
	FILE* fd = fopen(name,"r");
	if(fd->id){
		fclose(fd);
		return name;
	}
	char* buffer = append("/bin/",name);
	fd = fopen(buffer,"r");
	if(fd->id){
        fclose(fd);
        return buffer;
    }
	free(buffer);
	buffer = append("/sbin/",name);
    fd = fopen(buffer,"r");
    if(fd->id){
        fclose(fd);
        return buffer;
    }
	printf("File not found!\n");
	return 0x0;
}

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
			}else if(buffer[oldindex] == 0x1){
				buffer[oldindex] = 0x0;
				buffer[oldindex-1] = 0x0;
				index -= 2;
				fseek(stdout,-1,SEEK_CUR);
				fwrite(" ",1,1,stdout);
				fseek(stdout,-1,SEEK_CUR);
			}else if(buffer[oldindex] == 0x4){
				fseek(stdout,1,SEEK_CUR);
				index++;
			}else if(buffer[oldindex] == 0x5){
				fseek(stdout,-1,SEEK_CUR);
				index--;
			}
		}
		printf("\n");
		if(memcmp("cd ",buffer,3) == 0){
			// Command is to change dir
			uint64_t result = chdir(&buffer[3]);
			if(result != 1){
				printf("Error changing directory!\n");
			}
			continue;
		}
		int quote = 0;
		size_t len = strlen(buffer);
		size_t count = 0;
		for(size_t i = 0; i < len; i++){
			if(buffer[i] == '\'' || buffer[i] == '\"'){
				if(quote == 0)
					quote = 1;
				else
					quote = 0;
			}
			if(buffer[i] == ' ' && quote == 0){
				buffer[i] = 0; // Null terminate arguments
				count++;
			}
		}
		count++;
		// Make sure the end has atleast 2 null terminators
		buffer[len] = 0x0;
		size_t curr = 0;
		char** args = (char**) malloc(sizeof(char*)*count);
		for(size_t i = 0; i < count; i++){
			char* new_buffer = &buffer[curr];
			args[i] = new_buffer;
			curr += strlen(new_buffer)+1;
		}
		char* filename = find_file(buffer);
		if(filename == 0x0)
			continue;
		uint64_t pid = fork(); // Fork and execute new program
		if(pid == 0){
			chroot("/opt/");
			execv(filename,args);
			// If execution gets here then its not an executable file!
			printf("Invalid binary!\n");
			exit(1);
		}else{
			waitpid(pid);
		}
	}
}
