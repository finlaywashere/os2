#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/idt.h>
#include <process.h>
#include <safety.h>

void init_syscalls();

#endif
