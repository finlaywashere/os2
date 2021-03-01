#ifndef UTILS_H
#define UTILS_H

#include <stddef.h>
#include <stdint.h>

void memcpy(uint8_t* src, uint8_t* dst, uint64_t count);
uint64_t strlen(char* str);
int memcmp(uint8_t* mem1, uint8_t* mem2, uint64_t count);
void memset(uint8_t* buf, uint8_t val, uint64_t count);
int numlen(uint64_t num, uint64_t base);
void int_to_str(char* buffer, uint64_t number, uint64_t base);
void strcpy(char* src, char* dst);

// IO stuff

void outb(uint16_t port, uint8_t value);
uint8_t inb(uint16_t port);
void outw(uint16_t port, uint16_t value);
uint16_t inw(uint16_t port);
void outd(uint16_t port, uint32_t value);
uint32_t ind(uint16_t port);

#endif
