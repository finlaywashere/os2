#include <arch/x86_64/driver/timer/pit.h>

volatile uint64_t ticks = 0;
void init_pit(){
	int divisor = 1193182 / 100; // Trigger every 10ms

	outb(0x43, 0x36);

	// Divisor has to be sent in bytes
	uint8_t l = (uint8_t) (divisor & 0xFF);
	uint8_t h = (uint8_t) ((divisor >> 8) & 0xFF);

	outb(0x40, l);
	outb(0x40, h);
	
	isr_register_handler(0x20, &pit_count);
}
void pit_count(registers_t* regs){
	ticks++;
	schedule(regs);
}
uint64_t pit_get_ticks(){
	return ticks;
}
void pit_sleep(uint64_t ms){
	uint64_t endCount = ticks+ms;
	while(1){
		if(ticks >= endCount)
			return;
	}
}
