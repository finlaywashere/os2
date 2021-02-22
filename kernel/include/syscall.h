#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/idt.h>
#include <process.h>
#include <safety.h>

#define PROT_EXEC 1
#define PROT_READ 2
#define PROT_WRITE 4
#define PROT_NONE 8

#define MAP_ANONYMOUS 1

void init_syscalls();

#endif
