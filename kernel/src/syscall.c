#include <syscall.h>

syscall_t* handlers;

void syscall_write(registers_t* regs){
	uint64_t buffer_addr = regs->rbx;
	uint64_t count = regs->rcx;
	uint64_t descriptor = regs->rdx;
	if(usermode_buffer_safety(buffer_addr,count)){
		regs->rax = -1;
		return;
	}
	uint8_t* buffer = (uint8_t*) buffer_addr;
	process_t* process = get_process();
	if(value_safety(descriptor,0,process->count)){
		regs->rax = -2;
		return;
	}
	descriptor_t* desc = &process->descriptors[descriptor];
	if(desc->write == 0){
		regs->rax = -3;
		return;
	}
	uint64_t actual_count = desc->write(&process->descriptors[descriptor],buffer,count);
	regs->rax = actual_count;
}
void syscall_read(registers_t* regs){
	uint64_t buffer_addr = regs->rbx;
	uint64_t count = regs->rcx;
	uint64_t descriptor = regs->rdx;
	if(usermode_buffer_safety(buffer_addr,count)){
		regs->rax = -1;
		return;
	}
	uint8_t* buffer = (uint8_t*) buffer_addr;
	process_t* process = get_process();
	if(value_safety(descriptor, 0, process->count)){
		regs->rax = -2;
		return;
	}
	descriptor_t* desc = &process->descriptors[descriptor];
	if(desc->read == 0){
		regs->rax = -3;
		return;
	}
	uint64_t actual_count = desc->read(&process->descriptors[descriptor],buffer,count);
	regs->rax = actual_count;
}
void syscall_fork(registers_t* regs){
	fork_process(get_curr_process(), regs);
}
void syscall_exec(registers_t* regs){
	uint64_t buffer_addr = regs->rbx;
	char* buffer = (char*) buffer_addr;
	uint64_t count = strlen(buffer);
	if(usermode_buffer_safety(buffer_addr,count)){
		regs->rax = 0;
		return;
	}
	uint64_t process = get_curr_process();
	process_t* process_data = get_process();
	page_table_t* dst = (page_table_t*) kmalloc_p(sizeof(page_table_t));
	uint64_t entry_point = load_elf(buffer, dst);
	create_process_pid_nodesc(process, dst, entry_point,0,0,0,0,0);

	memcpy(&process_data->regs,regs,sizeof(registers_t));
}
void syscall_mmap(registers_t* regs){
	uint64_t addr = regs->rbx;
	uint64_t length = regs->rcx;
	uint32_t prot = (uint32_t) regs->rdx;
	uint32_t flags = (uint32_t) (regs->rdx >> 32);
	uint64_t descriptor = regs->r8;
	uint64_t offset = regs->r9;

	if(usermode_buffer_safety(addr,length)){
		regs->rax = -1;
		return;
	}
	
	if(flags & MAP_ANONYMOUS){
		uint64_t size_rounded = length-length%0x200000+(length%0x200000 > 0 ? 0x200000 : 0);
		uint64_t alloc_addr = kmalloc_pa(size_rounded,0x200000);
		uint32_t page_flags = 0b101;
		if(flags & PROT_WRITE)
			page_flags |= 0b10;
		
		map_pages(get_physical_addr(alloc_addr),addr,0b111,size_rounded);
		return;
	}else{
		process_t* process = get_process();
		if(value_safety(descriptor, 0, process->count)){
			regs->rax = -2;
			return;
		}
		//TODO: Implement file mmap stuff
		return;
	}
}
void syscall_fopen(registers_t* regs){
	uint64_t buffer = regs->rbx;
	char* name = (char*) buffer;
	uint64_t len = strlen(name);
	uint64_t mode = regs->rcx;
	
	if(usermode_buffer_safety(name,len)){
		regs->rax = 0;
		return;
	}

	uint64_t descriptor = open_file_descriptor(name,mode);

	regs->rax = descriptor;

	return;
}
void syscall_fseek(registers_t* regs){
	uint64_t id = regs->rbx;
	signed long offset = regs->rcx;
	uint64_t from = regs->rdx;
	process_t* process = get_process();
	
	if(value_safety(id, 0, process->count)){
		regs->rax = -1;
		return;
	}

	descriptor_t* desc = get_descriptor(id);

	uint64_t actual_offset = 0;
	if(from == 0){
		//SEEK_SET
		actual_offset = offset;
	}else if(from == 1){
		//SEEK_CUR
		actual_offset = desc->buffer_seek + offset;
	}else if(from == 2){
		//SEEK_END
		actual_offset = desc->buffer_size + offset;
	}else{
		regs->rax = -1;
		return;
	}

	if((actual_offset >= desc->buffer_size || actual_offset < 0) && id != 0){
		regs->rax = -1;
		return;
	}

	desc->buffer_seek = actual_offset;
	
	if(id == 0)
		tty_update_cursor(0, desc->buffer_seek);

	return;
}
void syscall_ftell(registers_t* regs){
	uint64_t id = regs->rbx;
	process_t* process = get_process();

	if(value_safety(id, 0, process->count)){
		regs->rax = -1;
		return;
	}

	descriptor_t* desc = get_descriptor(id);
	regs->rax = desc->buffer_seek;

	return;
}
void syscall_fclose(registers_t* regs){
	uint64_t id = regs->rbx;
	if(value_safety(id, 0, get_process()->count)){
		regs->rax = -1;
		return;
    }
	close_file_descriptor(id);
}
void syscall_exit(registers_t* regs){
	get_process()->regs.rax = regs->rbx;
	uint64_t curr = get_curr_process();
	schedule(regs);
	kill_process(curr);
}
void syscall_uname(registers_t* regs){
	uint64_t buffer = regs->rbx;
	char* name = (char*) buffer;
	uint64_t len = strlen(KERNEL_NAME);
	if(usermode_buffer_safety(name,len)){
		regs->rax = 0;
		return;
	}
	strcpy(KERNEL_NAME,name);
	regs->rax = 1;
}
void syscall_sysinfo(registers_t* regs){}
void syscall_rmdir(registers_t* regs){}
void syscall_rename(registers_t* regs){}
void syscall_readdir(registers_t* regs){
	uint64_t buffer_addr = regs->rbx;
    uint64_t count = regs->rcx;
    uint64_t descriptor = regs->rdx;
    if(usermode_buffer_safety(buffer_addr,count)){
        regs->rax = -1;
        return;
    }
    uint8_t* buffer = (uint8_t*) buffer_addr;
    process_t* process = get_process();
    if(value_safety(descriptor, 0, process->count)){
        regs->rax = -2;
        return;
    }
	fs_node_t* file = get_descriptor_file(descriptor);
	if(file->type != 0){
		regs->rax = -3;
		return;
	}
	uint64_t file_count = file->dir_entries(file);
	if(count > file_count)
		count = file_count;
	fs_node_t* k_buffer = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*file_count);
	file->read_dir(file,k_buffer);

	user_fs_node_t* u_buffer = (user_fs_node_t*) buffer;
	for(int i = 0; i < count; i++){
		fs_node_t f = k_buffer[i];
		u_buffer[i].flags = f.flags;
		u_buffer[i].type = f.type;
		u_buffer[i].inode = f.inode;
		u_buffer[i].creation_time = f.creation_time;
		u_buffer[i].modification_time = f.modification_time;
		u_buffer[i].length = f.length;
		memcpy(&f.name,&u_buffer[i].name,20);
	}
    regs->rax = count;
}
void syscall_readlink(registers_t* regs){}
void syscall_mkdir(registers_t* regs){}
void syscall_mknod(registers_t* regs){}
void syscall_link(registers_t* regs){}
void syscall_kill(registers_t* regs){
	if(value_safety(regs->rbx, 0, MAX_PROCESS_COUNT)){
		regs->rax = -1;
		return;
	}
	kill_process(regs->rbx);
}
void syscall_getuid(registers_t* regs){
	regs->rax = get_process()->uid;
}
void syscall_getgid(registers_t* regs){
	regs->rax = get_process()->gid;
}
void syscall_getpid(registers_t* regs){
	regs->rax = get_curr_process();
}
void syscall_setuid(registers_t* regs){
	get_process()->uid = regs->rbx;
}
void syscall_setgid(registers_t* regs){
	get_process()->gid = regs->rbx;
}
void syscall_chroot(registers_t* regs){}
void syscall_chdir(registers_t* regs){}
void syscall_unlink(registers_t* regs){}

void syscall(registers_t* regs){
	asm volatile("cli");
	if(regs->rax > 63 || regs -> rax < 0){
		regs->rax = 0;
		asm volatile("sti");
		return;
	}
	handlers[regs->rax](regs);
	asm volatile("sti");
}
void register_syscall(uint64_t id, syscall_t handler){
	handlers[id] = handler;
}
void init_syscalls(){
	handlers = kmalloc_p(sizeof(syscall_t)*64); // Max of 64 syscalls
	
	register_syscall(1,&syscall_write);
	register_syscall(2,&syscall_read);
	register_syscall(3,&syscall_fork);
	register_syscall(4,&syscall_exec);
	register_syscall(5,&syscall_mmap);
	register_syscall(6,&syscall_fopen);
	register_syscall(7,&syscall_fseek);
	register_syscall(8,&syscall_ftell);
	register_syscall(9,&syscall_fclose);
	register_syscall(10,&syscall_exit);
	register_syscall(11,&syscall_uname);
	register_syscall(12,&syscall_sysinfo);
	register_syscall(13,&syscall_rmdir);
	register_syscall(14,&syscall_rename);
	register_syscall(15,&syscall_readdir);
	register_syscall(16,&syscall_readlink);
	register_syscall(17,&syscall_mkdir);
	register_syscall(18,&syscall_mknod);
	register_syscall(19,&syscall_link);
	register_syscall(20,&syscall_kill);
	register_syscall(21,&syscall_getuid);
	register_syscall(22,&syscall_getgid);
	register_syscall(23,&syscall_getpid);
	register_syscall(24,&syscall_setuid);
	register_syscall(25,&syscall_setgid);
	register_syscall(26,&syscall_chroot);
	register_syscall(27,&syscall_chdir);
	register_syscall(28,&syscall_unlink);
	isr_register_handler(80, &syscall);
}
