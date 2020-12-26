#include <utils.h>

void memcpy(uint8_t* src, uint8_t* dst, uint64_t count){
	for(uint64_t i = 0; i < count; i++){
		dst[i] = src[i];
	}
}
uint64_t strlen(char* str){
	uint64_t len = 0;
	while(1){
		if(str[len] == 0x0)
			return len;
		len++;
	}
}
