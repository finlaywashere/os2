#ifndef KEYBOARD_H
#define KEYBOARD_H

#include <stdint.h>
#include <stddef.h>
#include <mem/vmm.h>
#include <utils.h>
#include <arch/x86_64/idt.h>

#define DATA_PORT 0x60
#define COMMAND_PORT 0x64

#define PS2_KEYBOARD 1
#define PS2_MOUSE 2

struct ps2_device{
	uint8_t type;
	uint8_t id;
};
typedef struct ps2_device ps2_device_t;

void init_keyboard();
uint8_t read_ps2_keyboard();
#endif
