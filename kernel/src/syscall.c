#include <syscall.h>

void syscall(registers_t* regs){
	asm volatile("cli");
	if(regs->rax == 1){
		// Write
		uint64_t buffer_addr = regs->rbx;
		uint64_t count = regs->rcx;
		uint64_t descriptor = regs->rdx;
		if(usermode_buffer_safety(buffer_addr,count)){
			regs->rax = -1;
			asm volatile("sti");
			return;
		}
		uint8_t* buffer = (uint8_t*) buffer_addr;
		process_t* process = get_process();
		if(value_safety(descriptor,0,process->count)){
			regs->rax = -2;
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
		if(usermode_buffer_safety(buffer_addr,count)){
			regs->rax = -1;
			asm volatile("sti");
			return;
		}
		uint8_t* buffer = (uint8_t*) buffer_addr;
		process_t* process = get_process();
		if(value_safety(descriptor, 0, process->count)){
			regs->rax = -2;
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
	}else if(regs->rax == 4){
		// Exec
		uint64_t buffer_addr = regs->rbx;
		char* buffer = (char*) buffer_addr;
		uint64_t count = strlen(buffer);
		if(usermode_buffer_safety(buffer_addr,count)){
			regs->rax = 0;
			asm volatile("sti");
			return;
		}
		uint64_t process = get_curr_process();
		process_t* process_data = get_process();
		page_table_t* dst = (page_table_t*) kmalloc_p(sizeof(page_table_t));
		uint64_t entry_point = load_elf(buffer, dst);
		create_process_pid_nodesc(process, dst, entry_point,0,0,0,0);
		
		memcpy(&process_data->regs,regs,sizeof(registers_t));
		
		asm volatile("sti");
		return;
	}else if(regs->rax == 5){
		//Mmap
		uint64_t addr = regs->rbx;
		uint64_t length = regs->rcx;
		uint32_t prot = (uint32_t) regs->rdx;
		uint32_t flags = (uint32_t) (regs->rdx >> 32);
		uint64_t descriptor = regs->r8;
		uint64_t offset = regs->r9;
		
		if(usermode_buffer_safety(addr,length)){
            regs->rax = -1;
            asm volatile("sti");
            return;
        }
		
		if(flags & MAP_ANONYMOUS){
			uint64_t size_rounded = length-length%0x200000+(length%0x200000 > 0 ? 0x200000 : 0);
			uint64_t alloc_addr = kmalloc_pa(size_rounded,0x200000);
			uint32_t page_flags = 0b101;
			if(flags & PROT_WRITE)
				page_flags |= 0b10;
			map_pages(get_physical_addr(alloc_addr),addr,0b111,size_rounded);
			asm volatile("sti");
			return;
		}else{
			process_t* process = get_process();
			if(value_safety(descriptor, 0, process->count)){
            	regs->rax = -2;
           		asm volatile("sti");
            	return;
        	}
			//TODO: Implement file mmap stuff
			asm volatile("sti");
			return;
		}
	}
}

void init_syscalls(){
	isr_register_handler(80, &syscall);
}
