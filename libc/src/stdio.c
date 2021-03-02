#include <stdio.h>

int printf(const char* str, ...){
	return fprintf(stdout,str);
}
size_t fwrite(const void* buffer, size_t element_size, size_t num_elements, FILE* file){
	uint64_t result = syscall(1,(uint64_t) buffer, (uint64_t) (element_size*num_elements), file->id,0,0);
	return (size_t) (result/element_size);
}
int fclose(FILE* file){
	syscall(9,file->id,0,0,0,0);
	return 0;
}
int fflush(FILE* file){
	//TODO: Implement fflush
	//NOTE: Kernel uses sync I/O so this probably won't do anything
	return 0;
}
FILE* fopen(const char* filename, const char* mode){
	uint64_t mode_int = 0;
	if(mode == "w+" || mode == "w")
			mode_int |= 1;
	FILE* ret = (FILE*) malloc(sizeof(FILE));
	ret->id = syscall(6,(uint64_t) filename, mode_int, 0, 0, 0);
	return ret;
}
int fprintf(FILE* file, const char* str, ...){
	va_list vl;
    int i = 0, j = 0;
    char buff[100]={0}, tmp[20];
    va_start(vl, str);
    while(str && str[i]){
        if(str[i] == '%'){
            i++;
            switch(str[i]){
                case 'c':
                    buff[j] = (char) va_arg(vl,int);
                    j++;
                    break;
                case 'd':
                    //TODO: Implement decimal stuff
                    j++;
                    break;
                case 'x':
                    //TODO: Implement hex stuff
                    j++;
                    break;
            }
        }else{
            buff[j] = str[i];
            j++;
        }
        i++;
    }
    int result = fwrite(buff,j,1,file);
    va_end(vl);
    return result-j;
}
size_t fread(void* buffer, size_t element_size, size_t num_elements, FILE* file){
	uint64_t result = syscall(2,(uint64_t) buffer, (uint64_t) (element_size*num_elements), file->id,0,0);
    return (size_t) (result/element_size);
}
int fseek(FILE* file, long offset, int whence){
	return syscall(7,file->id,offset,(uint64_t) whence,0,0);
}
long ftell(FILE* file){
	return syscall(8,file->id,0,0,0,0);
}
void setbuf(FILE* file, char* buffer){
	//TODO: Implement setbuf (sets a file descriptor's buffer)
}
int vfprintf(FILE* file, const char* str, va_list args){
	//TODO: Implement vfprintf
}
