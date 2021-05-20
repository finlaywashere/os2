#include <stddef.h>
#include <stdint.h>

struct cpuid{
	uint32_t ebx, edx, ecx;
}__attribute__((packed));

typedef struct cpuid cpuid_t;

int check_cpuid();
uint64_t cpuid(uint64_t rax, cpuid_t* dst);
