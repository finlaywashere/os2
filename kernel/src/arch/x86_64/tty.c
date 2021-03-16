#include <arch/x86_64/tty.h>
#include <font.h>

const int VGA_WIDTH = 640/8;
const int VGA_HEIGHT = 480/8;
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int VGA_PIXELS = 640*480;

const uint64_t raw_buffer = 0xFFFFFF80000A0000;

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
	for(int x = 0; x < 320; x++){
		for(int y = 0; y < 240; y++){
			ttys[0].buffer[y*320+x] = 1 | (1 << 4);
		}
	}
}
void tty_draw_pixel(uint64_t tty, uint16_t row, uint16_t col, uint8_t colour){
	int index = row * SCREEN_WIDTH + col;
	ttys[tty].buffer[index] = 1; //colour;
}
void set_tty(uint64_t tty){
	active_tty = tty;
}
void tty_putchar_raw(uint64_t tty, uint16_t row, uint16_t col, uint8_t colour, char character){
	for(int x = 0; x < 8; x++){
		for(int y = 0; y < 8; y++){
			tty_draw_pixel(tty,row+y,col+x,font[character][x*8+y]);
		}
	}
}
void tty_putchar(uint64_t tty, char character){
	if(character == '\n'){
		ttys[tty].col = 0;
		ttys[tty].row++;
		return;
	}
	uint16_t row = ttys[tty].row;
	uint16_t col = ttys[tty].col;
	uint8_t colour = ttys[tty].colour;
	tty_putchar_raw(tty,row,col,colour,character);
	ttys[tty].col++;
	if(ttys[tty].col >= VGA_WIDTH){
		ttys[tty].col = 0;
		ttys[tty].row++;
	}
}
void tty_putchars(uint64_t tty, char* characters, uint64_t count){
	for(uint64_t i = 0; i < count; i++){
		char c = characters[i];
		tty_putchar(tty,c);
	}
}
uint64_t tty_putchars_raw(uint64_t tty, char* characters, uint64_t count, uint64_t seek){
	ttys[tty].row = seek / VGA_WIDTH % VGA_HEIGHT;
	ttys[tty].col = seek % VGA_WIDTH;
    for(uint64_t i = 0; i < count; i++){
        char c = characters[i];
        tty_putchar(tty,c);
    }
	return ttys[tty].row * VGA_WIDTH + ttys[tty].col;
}

void tty_writestring(uint64_t tty, char* str){
	uint64_t len = strlen(str);
	tty_putchars(tty,str,len);
}
void tty_update_cursor(uint64_t tty, uint64_t seek){
	ttys[tty].row = seek / VGA_WIDTH % VGA_HEIGHT;
	ttys[tty].col = seek % VGA_WIDTH;
	uint16_t pos = ttys[tty].row * VGA_WIDTH + ttys[tty].col;
	outb(0x3D4, 0x0F);
	outb(0x3D5, (uint8_t) (pos & 0xFF));
	outb(0x3D4, 0x0E);
	outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
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
void tty_setcolour(uint64_t tty, uint8_t foreground, uint8_t background){
	uint8_t colour = (((uint16_t)background) << 4) | foreground;
	ttys[tty].colour = colour;
}
uint8_t tty_getcolour(uint64_t tty){
	return ttys[tty].colour;
}
