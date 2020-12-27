#include <arch/x86_64/tty.h>

const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;
const int VGA_PIXELS = VGA_WIDTH*VGA_HEIGHT;

const uint64_t raw_buffer = (uint16_t*)0xFFFFFF80000B8000;

uint64_t active_tty = 0;

tty_t* ttys;

void init_ttys(int tty_count){
	ttys = (tty_t*) kmalloc_p(sizeof(tty_t)*tty_count);
	for(int i = 0; i < tty_count; i++){
		init_tty(i);
	}
}

void init_tty(int number){
	ttys[number].row = 0;
	ttys[number].col = 0;
	ttys[number].colour = 0xF; // White foreground and black background
	if(number != 0)
		ttys[number].buffer = (uint16_t*) kmalloc_p(sizeof(uint16_t)*VGA_PIXELS);
	else
		ttys[number].buffer = (uint16_t*) raw_buffer;
}
void set_tty(uint64_t tty){
	active_tty = tty;
}
void tty_putchar(uint64_t tty, uint16_t row, uint16_t col, uint8_t colour, char character){
	int index = row * VGA_WIDTH + col;
	ttys[tty].buffer[index] = ((uint16_t)character) | (colour << 8);
}
void tty_putchars(uint64_t tty, char* characters, uint64_t count){
	for(uint64_t i = 0; i < count; i++){
		uint16_t row = ttys[tty].row;
		uint16_t col = ttys[tty].col;
		uint8_t colour = ttys[tty].colour;
		tty_putchar(tty,row,col,colour,characters[i]);
		ttys[tty].col++;
		if(ttys[tty].col >= VGA_WIDTH){
			ttys[tty].col = 0;
			ttys[tty].row++;
		}
	}
}
void tty_writestring(uint64_t tty, char* str){
	uint64_t len = strlen(str);
	tty_putchars(tty,str,len);
}
/*
Copy's a TTY to another TTY
-1 tty code indicates the raw VGA buffer
*/
void tty_copy(uint64_t src_tty, uint64_t dst_tty){
	uint16_t* src_buffer = ttys[src_tty].buffer;
	uint16_t* dst_buffer = ttys[dst_tty].buffer;
	memcpy((uint8_t*)src_buffer,(uint8_t*)dst_buffer, VGA_PIXELS*2); // Convert shorts to bytes!
}
