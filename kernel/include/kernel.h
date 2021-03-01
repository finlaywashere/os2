#ifndef KERNEL_H
#define KERNEL_H

#include <arch/x86_64/tty.h>
#include <mem/pmm.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/acpi.h>
#include <log.h>
#include <arch/x86_64/idt.h>
#include <arch/x86_64/driver/pci.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <arch/x86_64/driver/timer/timer.h>
#include <arch/x86_64/driver/disk/ide.h>
#include <fs/fs.h>
#include <elf.h>
#include <process.h>
#include <syscall.h>

#define KERNEL_NAME "FOS2 V1.0"

void kernel_start();

#endif
