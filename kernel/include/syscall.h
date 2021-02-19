#pragma once

#include <stddef.h>
#include <stdint.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/idt.h>

void init_syscalls();
