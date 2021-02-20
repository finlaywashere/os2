#include <syscall.h>

void syscall(registers_t* regs){
	asm volatile("cli");
	if(regs->rax == 1){
		// Write
		uint64_t buffer_addr = regs->rbx;
		uint64_t count = regs->rcx;
		uint64_t descriptor = regs->rdx;
		if(buffer_addr < 0 || buffer_addr >= CUTOFF_MEM || count < 0 || count > CUTOFF_MEM || buffer_addr + count > CUTOFF_MEM){
			regs->rax = 0;
			regs->rbx = 1;
			asm volatile("sti");
			return;
		}
		uint8_t* buffer = (uint8_t*) buffer_addr;
		process_t* process = get_process();
		if(count < 0 || count > process->count){
			regs->rax = 0;
			regs->rbx = 2;
			asm volatile("sti");
			return;
		}
		uint64_t actual_count = process->descriptors[descriptor].write(&process->descriptors[descriptor],buffer,count);
		regs->rax = actual_count;
		asm volatile("sti");
		return;
	}else if(regs->rax == 2){
		// Read
                uint64_t buffer_addr = regs->rbx;
                uint64_t count = regs->rcx;
                uint64_t descriptor = regs->rdx;
                if(buffer_addr < 0 || buffer_addr >= CUTOFF_MEM || count < 0 || count > CUTOFF_MEM || buffer_addr + count > CUTOFF_MEM){
                        regs->rax = 0;
                        regs->rbx = 1;
			asm volatile("sti");
                        return;
                }
                uint8_t* buffer = (uint8_t*) buffer_addr;
                process_t* process = get_process();
                if(count < 0 || count > process->count){
                        regs->rax = 0;
                        regs->rbx = 2;
			asm volatile("sti");
                        return;
                }
                uint64_t actual_count = process->descriptors[descriptor].read(&process->descriptors[descriptor],buffer,count);
                regs->rax = actual_count;
		asm volatile("sti");
                return;
        }else if(regs->rax == 3){
		// Fork
		fork_process(get_curr_process(), regs);
		asm volatile("sti");
		return;
	}
}

void init_syscalls(){
	isr_register_handler(80, &syscall);
}
