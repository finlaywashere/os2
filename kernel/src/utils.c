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
int memcmp(uint8_t* mem1, uint8_t* mem2, uint64_t count){
	for(uint64_t i = 0; i < count; i++){
		if(mem1[i] != mem2[i])
			return 0;
	}
	return 1;
}

// IO stuff
void outb(uint16_t port, uint8_t value){
	asm volatile ("outb %1, %0" : : "dN" (port), "a" (value));
}

uint8_t inb(uint16_t port){
	uint8_t ret;
	asm volatile("inb %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}
void outw(uint16_t port, uint16_t value){
	asm volatile ("outw %1, %0" : : "dN" (port), "a" (value));
}

uint16_t inw(uint16_t port){
	uint16_t ret;
	asm volatile("inw %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}
void outd(uint16_t port, uint32_t value){
	asm volatile ("out %1, %0" : : "dN" (port), "a" (value));
}
uint32_t ind(uint16_t port){
	uint32_t ret;
	asm volatile("in %1, %0" : "=a" (ret) : "dN" (port));
	return ret;
}

