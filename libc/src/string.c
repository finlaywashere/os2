#include <string.h>

void* memcpy(void* dst, const void* src, size_t len){
	uint8_t* dst_array = (uint8_t*) dst;
	uint8_t* src_array = (uint8_t*) src;
	for(size_t i = 0; i < len; i++){
		dst_array[i] = src_array[i];
	}
	return dst;
}
void* memset(void* addr, int value, size_t len){
    uint8_t* addr_array = (uint8_t*) addr;
    for(size_t i = 0; i < len; i++){
        addr_array[i] = (uint8_t) value;
    }
	return addr;
}
char* strcpy(char* dst, const char* src){
	size_t src_len = strlen(src);
	for(size_t i = 0; i < src_len; i++){
		dst[i] = src[i];
	}
	return dst;
}
size_t strlen(const char* str){
	size_t len = 0;
	while(1){
		if(str[len] == 0)
			return len;
		len++;
	}
}
