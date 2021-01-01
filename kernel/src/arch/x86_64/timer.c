#include <arch/x86_64/timer.h>

void init_timers(){
	init_pit();
}
uint64_t get_time(){
	return pit_get_ticks();
}
void sleep(uint64_t ms){
	pit_sleep(ms);
}
