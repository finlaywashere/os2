#ifndef KERNEL_H
#define KERNEL_H

#include <mem/pmm.h>
#include <log.h>
#include <fs/fs.h>
#include <elf.h>
#include <process.h>
#include <syscall.h>
#include <arch/x86_64/arch.h>

#define KERNEL_NAME "FOS2 V1.0"

void kernel_start();

#endif
