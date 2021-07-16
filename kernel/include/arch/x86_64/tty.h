#ifndef TTY_H
#define TTY_H

#include <stdint.h>
#include <stddef.h>
#include <mem/vmm.h>
#include <utils.h>

enum vga_colour {
        VGA_COLOUR_BLACK = 0x0,
        VGA_COLOUR_BLUE = 0x1,
        VGA_COLOUR_GREEN = 0x2,
        VGA_COLOUR_CYAN = 0x3,
        VGA_COLOUR_RED = 0x4,
        VGA_COLOUR_MAGENTA = 0x5,
        VGA_COLOUR_BROWN = 0x6,
        VGA_COLOUR_LIGHT_GRAY = 0x7,
        VGA_COLOUR_DARK_GRAY = 0x8,
        VGA_COLOUR_LIGHT_BLUE = 0x9,
        VGA_COLOUR_LIGHT_GREEN = 0xA,
        VGA_COLOUR_LIGHT_CYAN = 0xB,
        VGA_COLOUR_LIGHT_RED = 0xC,
        VGA_COLOUR_LIGHT_MAGENTA = 0xD,
        VGA_COLOUR_YELLOW = 0xE,
        VGA_COLOUR_WHITE = 0xF,
};
struct tty{
	uint16_t row;
	uint16_t col;
	uint8_t colour;
	uint16_t* buffer;
};
typedef struct tty tty_t;

void init_ttys(int count);
void init_tty(int number);

void tty_putchar_raw(uint64_t tty, uint16_t row, uint16_t col, uint8_t colour, char character);
void tty_putchar(uint64_t tty, char character);
void set_tty(uint64_t tty);
void tty_copy(uint64_t src_tty, uint64_t dst_tty);
void tty_putchars(uint64_t tty, char* characters, uint64_t count);
void tty_writestring(uint64_t tty, char* str);
void tty_setcolour(uint64_t tty, uint8_t foreground, uint8_t background);
uint8_t tty_getcolour(uint64_t tty);
uint64_t tty_putchars_raw(uint64_t tty, char* buffer, uint64_t count, uint64_t seek);
void tty_update_cursor(uint64_t tty, uint64_t seek);
void tty_clear(uint64_t tty);
#endif
