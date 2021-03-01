#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/idt.h>
#include <process.h>
#include <safety.h>
#include <fs/fs.h>
#include <kernel.h>


#define PROT_EXEC 1
#define PROT_READ 2
#define PROT_WRITE 4
#define PROT_NONE 8

#define MAP_ANONYMOUS 1

typedef void (*syscall_t)(registers_t*);

void init_syscalls();
void register_syscall(uint64_t id, syscall_t handler);

#endif
