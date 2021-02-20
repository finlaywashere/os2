#pragma once

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/idt.h>
#include <process.h>

#define CUTOFF_MEM 0xffff000000000000

void init_syscalls();
