#ifndef ARCH_H
#define ARCH_H

#include <stdint.h>
#include <stddef.h>

#include <arch/x86_64/idt.h>
#include <arch/x86_64/gdt.h>
#include <arch/x86_64/paging.h>
#include <arch/x86_64/acpi.h>
#include <arch/x86_64/tty.h>
#include <arch/x86_64/driver/driver.h>

void init_arch();

#endif
