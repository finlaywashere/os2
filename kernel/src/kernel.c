#include <stdint.h>
#include <stddef.h>

static const uint16_t *VGA_MEMORY = (uint16_t*) 0xB8000;
static const VGA_WIDTH = 80;
static const VGA_HEIGHT = 25;

uint16_t* terminal_buffer;

size_t terminal_row, terminal_col;

void terminal_init(){
	terminal_buffer = VGA_MEMORY;
	terminal_row = 0;
	terminal_col = 0;
}
void terminal_putchar(char c){
	unsigned char uc = c;
	size_t index = terminal_row * VGA_WIDTH + terminal_col;
	terminal_buffer[index] = ((uint16_t)uc) | (7 | 0<<4) << 8;
	terminal_col++;
}

void terminal_write(const char* data, size_t size){
	for(size_t i = 0; i < size; i++){
		terminal_putchar(data[i]);
	}
}

void kernel_start(){
	terminal_init();
	terminal_write("Successfully booted kernel!",28);
}
