#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stddef.h>

#include <arch/x86_64/driver/keyboard.h>
#include <arch/x86_64/driver/pci.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <arch/x86_64/driver/timer/timer.h>
#include <arch/x86_64/driver/disk/ide.h>
#include <arch/x86_64/driver/video/vga.h>

void init_drivers();

#endif
