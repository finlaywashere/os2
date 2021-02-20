#include <syscall.h>

void syscall(registers_t* regs){
	if(regs->rax == 1){
		uint64_t buffer_addr = regs->rbx;
		uint64_t count = regs->rcx;
		uint64_t descriptor = regs->rdx;
		if(buffer_addr < 0 || buffer_addr >= CUTOFF_MEM || count < 0 || count > CUTOFF_MEM || buffer_addr + count > CUTOFF_MEM){
			regs->rax = 0;
			regs->rbx = 1;
			return;
		}
		uint8_t* buffer = (uint8_t*) buffer_addr;
		process_t* process = get_process();
		if(count < 0 || count > process->count){
			regs->rax = 0;
			regs->rbx = 2;
			return;
		}
		uint64_t count = process->descriptors[descriptor].write(&process->descriptors[descriptor],buffer,count);
		regs->rax = count;
		return;
	}
}

void init_syscalls(){
	isr_register_handler(80, &syscall);
}
