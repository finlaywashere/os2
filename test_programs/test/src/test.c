#include <stdio.h>
#include <stdlib.h>

int main(){
	char buf[1];
	buf[0] = 0x0;
	for(uint64_t i = 0; i < 80*25; i++){
		fwrite(&buf,1,1,stdout);
	}
	while(1){
		char buffer[1];
		fread(&buffer,1,1,stdin);
		if(buffer[0] > 0x10){
			fwrite(&buffer,1,1,stdout);
		}else if(buffer[0] == 0x1){
			fseek(stdout,-1,SEEK_CUR);
			fwrite(" ",1,1,stdout);
			fseek(stdout,-1,SEEK_CUR);
		}else if(buffer[0] == 0x2){
			fseek(stdout,-80,SEEK_CUR);
		}else if(buffer[0] == 0x3){
			fseek(stdout,80,SEEK_CUR);
		}else if(buffer[0] == 0x4){
			fseek(stdout,1,SEEK_CUR);
		}else if(buffer[0] == 0x5){
			fseek(stdout,-1,SEEK_CUR);
		}
	}
	while(1);
}
