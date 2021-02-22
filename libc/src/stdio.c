#include <stdio.h>

int printf(const char* str, ...){
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
	int result = fwrite(buff,j,1,stdout);
	va_end(vl);
	return result-j;
}
size_t fwrite(const void* buffer, size_t element_size, size_t num_elements, FILE* file){
	uint64_t result = syscall(1,(uint64_t) buffer, (uint64_t) (element_size*num_elements), file->id);
	return (size_t) result;
}
