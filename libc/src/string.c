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
int strcmp(const char* str1, const char* str2){
	size_t len1 = strlen(str1);
	size_t len2 = strlen(str2);

	if(len1 < len2){
		return -1;
	}else if(len1 > len2){
		return 1;
	}
	for(size_t i = 0; i < len1; i++){
		if(str1[i] < str2[i]){
			return -1;
		}else if(str1[i] > str2[i]){
			return 1;
		}
	}
	return 0;
}
int memcmp(const void* str1, const void* str2, size_t count){
	char* str1_c = (char*) str1;
	char* str2_c = (char*) str2;
	for(size_t i = 0; i < count; i++){
        if(str1_c[i] < str2_c[i]){
            return -1;
        }else if(str1_c[i] > str2_c[i]){
            return 1;
        }
    }
    return 0;
}
