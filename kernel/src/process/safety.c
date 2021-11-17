#include <process/safety.h>

int address_safety(uint64_t addr){
	if(addr < 0 || addr >= CUTOFF_MEM){
		return 1;
	}
	return 0;
}

int usermode_buffer_safety(uint64_t addr, uint64_t count){
	if(addr < 0 || addr >= CUTOFF_MEM || count < 0 || count > CUTOFF_MEM || addr + count > CUTOFF_MEM){
		return 1;
	}
	return 0;
}
int value_safety(uint64_t value, uint64_t min, uint64_t max){
	if(value < min || value > max)
		return 1;
	return 0;
}
