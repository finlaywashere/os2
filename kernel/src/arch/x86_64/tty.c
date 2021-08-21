#include <arch/x86_64/tty.h>

const int VGA_WIDTH = 80;
const int VGA_HEIGHT = 25;
const int VGA_PIXELS = VGA_WIDTH*VGA_HEIGHT;

const uint64_t raw_buffer = 0xFFFFFF80000B8000;

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
void tty_scroll(uint64_t tty, uint64_t rows){
	ttys[tty].row -= rows;
	for(int y = rows; y < VGA_HEIGHT; y++){
		for(int x = 0; x < VGA_WIDTH; x++){
			int index = y * VGA_WIDTH + x;
			int new_index = (y - rows) * VGA_WIDTH + x;
			ttys[tty].buffer[new_index] = ttys[tty].buffer[index];
			ttys[tty].buffer[index] = 0x0;
		}
	}
}
void tty_putchar_raw(uint64_t tty, uint16_t row, uint16_t col, uint8_t colour, char character){
	int index = row * VGA_WIDTH + col;
	ttys[tty].buffer[index] = ((uint16_t)character) | (colour << 8);
	
}
void tty_clear(uint64_t tty){
	memset(ttys[tty].buffer,0,VGA_PIXELS*2); // Multiply by 2 because each pixel is a 16 bit int
	ttys[tty].row = 0;
	ttys[tty].col = 0;
	ttys[tty].colour = 0xF;
}
void tty_putchar(uint64_t tty, char character){
	if(character == '\n'){
		ttys[tty].col = 0;
		ttys[tty].row++;
		if(ttys[tty].row >= VGA_HEIGHT){
			tty_scroll(tty,1);
		}
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
	//ttys[tty].row = seek / VGA_WIDTH % VGA_HEIGHT;
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
	//ttys[tty].row = seek / VGA_WIDTH % VGA_HEIGHT;
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
