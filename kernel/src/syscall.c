#include <syscall.h>

void syscall(registers_t* regs){
	if(regs->rax == 1){
		tty_putchar(0,regs->rbx);
	}
}

void init_syscalls(){
	isr_register_handler(80, &syscall);
}
