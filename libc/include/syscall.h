#include <stdint.h>
#include <stddef.h>

extern uint64_t syscall(uint64_t rax, uint64_t rbx, uint64_t rcx, uint64_t rdx, uint64_t r8, uint64_t r9);

void exit(int code);
