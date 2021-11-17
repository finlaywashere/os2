#ifndef KERNEL_H
#define KERNEL_H

#include <mem/pmm.h>
#include <log.h>
#include <fs/fs.h>
#include <process/loader/elf.h>
#include <process/process.h>
#include <process/syscall.h>
#include <arch/x86_64/arch.h>

#define KERNEL_NAME "FOS2 V1.0"

void kernel_start(); // Starts the kernel

#endif
