#include <arch/x86_64/pit.h>

volatile uint64_t ticks = 0;
void init_pit(){
	int divisor = 1193182 / 1000; // Trigger every 1ms

	outb(0x43, 0x36);

	// Divisor has to be sent in bytes
	uint8_t l = (uint8_t) (divisor & 0xFF);
	uint8_t h = (uint8_t) ((divisor >> 8) & 0xFF);

	outb(0x40, l);
	outb(0x40, h);
}
void pit_count(){
	ticks++;
}
void sleep(uint64_t ms){
	uint64_t endCount = ticks+ms;
	while(1){
		if(ticks >= endCount)
			return;
	}
}
