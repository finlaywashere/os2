#include <arch/x86_64/driver/video/vga.h>

void init_vga(){
	outb(0x3C4,0x4); // Write index of smmr
	outb(0x3C5,1 << 3); // Set chain bit

	outb(0x3CE,0x5); // Write index of graphics mode register
	outb(0x3CF,1 << 6); // Set 256 colour shift bit
}
