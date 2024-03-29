#include <process/process.h>
#include <process/signal.h>

uint64_t curr_max_process = 0;
uint64_t curr_process = 0;

process_t* processes;

void init_processes(){
	processes = (process_t*) kmalloc_p(sizeof(process_t)*MAX_PROCESS_COUNT); // Max of MAX_PROCESS_COUNT of processes
	memset(processes,0,sizeof(process_t)*MAX_PROCESS_COUNT);
}
descriptor_t* get_descriptor(uint64_t id){
	return &processes[curr_process].descriptors[id];
}
process_t* get_process(){
	return &processes[curr_process];
}
uint64_t get_curr_process(){
	return curr_process;
}
int is_kernel(){
	return curr_max_process == 0;
}
void schedule(registers_t* regs){
	if(curr_max_process == 0)
		return; // There are no processes to be scheduled
	process_t *process = &processes[curr_process];
	// Save the registers from the current interrupt to the current process' state
	memcpy(&process->regs,regs,sizeof(registers_t));
	process->page_table = (uint64_t) get_curr_page_directory();
	// Mark the current process as no longer running if its not already been changed
	if(process->status == PROCESS_RUNNING)
		process->status = PROCESS_READY;
	// Go to the next process
	curr_process++;
	process = &processes[curr_process];
	// We may need to loop through all the processes to find the next one
	// so, the looped variable marks whether or not we have already hit the
	// max process and gone back around. This prevents infinite loops
	int looped = 0;
	// Loop until we find a process thats ready to execute
	while(process->status != PROCESS_READY){
		// Increment the current pid, the loop will exit when we find a valid one
		if(process->status == PROCESS_WAIT){
			scheduler_info_t sinfo = process->sch_info;
			if(sinfo.wait_type == WAIT_PID){
				if(processes[sinfo.wait_condition].status == PROCESS_DEAD){
					process->status = PROCESS_READY; // If the process it is waiting for is dead then resume
					break;
				}
			}
		}
		// If the current pid is above the max then loop back around to 1 (as 0 is the kernel process)
		if(curr_process >= MAX_PROCESS_COUNT && looped == 0){
			curr_process = 1;
			// Set looped so that this if will never execute again during this interrupt
			looped = 1;
			process = &processes[curr_process];
		}else if(curr_process >= MAX_PROCESS_COUNT){
			// No processes are available so switch to the kernel process
			curr_process = 0;
			break;
		}else{
			curr_process++;
	        process = &processes[curr_process];
		}
	}
	// Now the current PID is the one that will be running after the interrupt
	// Mark the current PID as running
	process->status = PROCESS_RUNNING;
	// Set the rsp for ring 0 in the TSS as this process' shadow stack to prevent stack collisions in the kernel
	tss_set_rsp(process->shadow_stack);
	// Copy the new process' saved state to the registers that will be set before executing the new process
	memcpy(regs,&process->regs,sizeof(registers_t));
	// Change the page directory to the one where the new process is mapped
	set_page_directory(process->page_table);
}
int process_error(registers_t *regs){
	if(curr_process == 0)
		return 1;
	
	int signal = get_error_signal();
	if((signed int) signal < 0){
		log_error("Process #");
		log_error_num(curr_process,10);
		log_error(" encountered an error and has been killed!\n");
		log_error("Error code 0x");
		log_error_num(regs->error,16);
		log_error(" ");
		log_error_num(regs->rip,16);
		log_error("!\n");
		// Kill process and schedule new one in its place
		kill_process(curr_process);
		schedule(regs);
		return 0;
	}
	uint64_t address = get_process()->signal_handlers[signal].handler;

	// error code from regs
	uint64_t error = regs->error;
	uint64_t cr2 = regs->cr2;
	uint64_t rip = regs->rip;
	
	regs->rip = address;
	
	regs->rdi = error;
	regs->rsi = rip;
	regs->rdx = cr2;

	return 0;
}
void process_wait(uint64_t pid, registers_t* regs){
	process_t *process = &processes[curr_process];
	process->sch_info.wait_type = WAIT_PID;
	process->sch_info.wait_condition = pid;
	process->status = PROCESS_WAIT;
	schedule(regs);
}
uint64_t read_file(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	if(descriptor == 0)
        return 0;
	uint64_t offset = descriptor->buffer_seek;
	
	fs_node_t* file = &processes[curr_process].files[descriptor->id]; // Grab file from the descriptor's internal id
	uint64_t read = file->read_file(file,offset,size,buffer);
	
	descriptor->buffer_seek += read;
	
	return read;
}
uint64_t write_file(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	if(descriptor == 0)
        return 0;
	uint64_t offset = descriptor->buffer_seek;

    fs_node_t* file = &processes[curr_process].files[descriptor->id]; // Grab file from the descriptor's internal id
    file->write_file(file,offset,size,buffer);
    
	descriptor->buffer_size = file->length;
	
	return size;
}
fs_node_t* get_descriptor_file(uint64_t id){
    descriptor_t desc = processes[curr_process].descriptors[id];
    if(desc.read != &read_file)
        return 0;
    return &processes[curr_process].files[desc.id];
}

uint64_t read(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	if(descriptor == 0)
		return 0;
	uint64_t seek = descriptor->buffer_seek;
	uint8_t* src_buffer = (uint8_t*) (((uint64_t) descriptor->buffer) + seek);
	uint64_t src_size = descriptor->buffer_size;
	if(seek >= size)
		return 0;
	uint64_t count = src_size - seek;
	if(count > size)
		count = size;
	descriptor->buffer_seek = seek+count;
	memcpy(buffer,src_buffer,count);
	return count;
}
uint64_t write(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	if(descriptor == 0)
        return 0;
	uint64_t dst_seek = descriptor->buffer_seek;
	uint8_t* dst_buffer = (uint8_t*) (((uint64_t) descriptor->buffer) + dst_seek);
	uint64_t dst_size = descriptor->buffer_size;
	if(dst_seek >= size)
		return 0;
	uint64_t count = dst_size - dst_seek;
	if(count > size)
		count = size;
	descriptor->buffer_seek = dst_seek+count;
	memcpy(dst_buffer,buffer,count);
	return count;
}
uint64_t write_screen(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	uint64_t new_seek = tty_putchars_raw(0, (char*) buffer, size,descriptor->buffer_seek);
	tty_update_cursor(0,descriptor->buffer_seek+1);
	descriptor->buffer_seek = new_seek;
	return size;
}
uint64_t read_keyboard(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	for(uint64_t i = 0; i < size; i++){
		buffer[i] = read_ps2_keyboard();
		if(buffer[i] == 0)
			return i;
	}
	return size;
}
uint8_t configure_descriptors(uint64_t pid, uint64_t parent){
	if(parent == 0){
		processes[pid].descriptors[0].write = &write_screen;
		processes[pid].descriptors[1].read = &read_keyboard;
	}else{
		uint64_t parent_descriptor_slot = 0;
		for(uint64_t i = 2; i < processes[parent].count; i++){
			if(processes[parent].descriptors[i].flags != DESCRIPTOR_PRESENT){
				parent_descriptor_slot = i;
				break;
			}
		}
		if(parent_descriptor_slot == 0)
			return -2;
		// Lowest is always the child's stdout (descriptor 0 on child process)
		/*processes[parent].descriptors[parent_descriptor_slot].read = &read;
		processes[parent].descriptors[parent_descriptor_slot].id = pid << 32;
		processes[pid].descriptors[0].buffer = (uint8_t*) kmalloc_p(BUFFER_SIZE);
		processes[pid].descriptors[0].buffer_size = BUFFER_SIZE;
		processes[pid].descriptors[0].write = &write;
		processes[pid].descriptors[0].id = parent << 32 | parent_descriptor_slot;
		// Highest is always the child's stdin (descriptor 1 on child process)
		processes[pid].descriptors[1].read = &read;
		processes[pid].descriptors[1].id = pid << 32;
		processes[parent].descriptors[parent_descriptor_slot+1].buffer = (uint8_t*) kmalloc_p(BUFFER_SIZE);
		processes[parent].descriptors[parent_descriptor_slot+1].buffer_size = BUFFER_SIZE;
		processes[parent].descriptors[parent_descriptor_slot+1].write = &write;
		processes[parent].descriptors[parent_descriptor_slot+1].id = pid << 32 | 1;*/
		processes[pid].descriptors[0].write = &write_screen;
        processes[pid].descriptors[1].read = &read_keyboard;
	}
	return 0;
}
void close_file_descriptor(uint64_t id){
	uint64_t file_id = processes[curr_process].descriptors[id].id;
	memset(&processes[curr_process].descriptors[id],0,sizeof(descriptor_t));
	memset(&processes[curr_process].files[file_id],0,sizeof(fs_node_t));
}
uint64_t open_file_descriptor(char* name, uint64_t mode){
	uint64_t desc_slot = 0;
	for(uint64_t i = 0; i < MAX_DESCRIPTOR_COUNT; i++){
		if(i < processes[curr_process].count){
			if(processes[curr_process].descriptors[i].id == 0 && processes[curr_process].descriptors[i].read == 0 && processes[curr_process].descriptors[i].write == 0){
				desc_slot = i;
				break;
			}
		}else{
			desc_slot = i;
			processes[curr_process].count++;
			break;
		}
	}
	if(desc_slot == 0)
		return 0;
	uint64_t file_slot = 0;
	for(uint64_t i = 1; i < MAX_FILE_COUNT; i++){
		if(processes[curr_process].files[i].name[0] == 0){
			file_slot = i;
			break;
		}
	}
	if(file_slot == 0)
		return 0;
	if(!(mode & MODE_WRITE)){
		get_file(name,&processes[curr_process].files[file_slot],processes[curr_process].current_directory); // Read file into file slot
		if(processes[curr_process].files[file_slot].name[0] == 0)
			return 0; // File not found
	}else{
		create_file(name,0,1,&processes[curr_process].files[file_slot]);
	}
	
	processes[curr_process].descriptors[desc_slot].id = file_slot;
	processes[curr_process].descriptors[desc_slot].buffer_size = processes[curr_process].files[file_slot].length;
	processes[curr_process].descriptors[desc_slot].read = &read_file;
	processes[curr_process].descriptors[desc_slot].write = &write_file;
	return desc_slot;
}
uint64_t find_slot(){
	uint64_t slot = 0;
        for(uint64_t i = 1; i < MAX_PROCESS_COUNT; i++){
                if(processes[i].status == PROCESS_DEAD){
                        slot = i;
                        break;
                }
        }
        if(slot == 0)
                return 0;
	return slot;
}
uint64_t fork_process(uint64_t parent, registers_t* regs){
	memcpy(&processes[parent].regs,regs,sizeof(registers_t));
	uint64_t child = find_slot();
	if(child == 0)
		return 0;
	memcpy(&processes[child],&processes[parent],sizeof(process_t));
	processes[parent].regs.rax = child;
	regs->rax = child;
	processes[child].regs.rax = 0;
	for(uint64_t i = 0; i < processes[child].count; i++){
		if(processes[child].descriptors[i].buffer != 0)
			processes[child].descriptors[i].buffer = (uint8_t*) kmalloc_p(BUFFER_SIZE);
	}
	page_table_t* new_page_table = hard_copy(processes[parent].page_table);
	processes[child].page_table = new_page_table;
	processes[child].shadow_stack = (uint64_t) kmalloc_p(STACK_SIZE);
	processes[child].parent = parent;
	processes[child].status = PROCESS_READY;
	curr_max_process++;
	return child;
}
uint64_t create_process(page_table_t* loaded_data, uint64_t entry, uint64_t parent, char* argv[], int argc, char* envp[], int envc, uint8_t interrupts){
	uint64_t slot = find_slot();
	if(slot == 0)
		return 0;
	create_process_pid(slot,loaded_data,entry,parent,argv,argc,envp,envc,interrupts);
	curr_max_process++;
	return slot;
}
void create_process_pid(uint64_t pid, page_table_t* loaded_data, uint64_t entry, uint64_t parent, char* argv[], int argc, char* envp[], int envc, uint8_t interrupts){
	asm volatile("cli");
	uint64_t slot = pid;
	
	create_process_pid_nodesc(pid,loaded_data,entry,argv,argc,envp,envc,interrupts);
	processes[slot].parent = parent;

	configure_descriptors(slot,parent);
	
	if(interrupts)
		asm volatile("sti");
	return slot;
}
void create_process_pid_nodesc(uint64_t pid, page_table_t* loaded_data, uint64_t entry, char* argv[], int argc, char* envp[], int envc, uint8_t interrupts){
	asm volatile("cli");
    uint64_t slot = pid;
	memset(&processes[slot].regs,0,sizeof(registers_t));
	
	page_table_t* curr_dir = get_curr_page_directory();
	set_page_directory(loaded_data);

	// Setup process' stack
	// 2MiB stack for convenience
	uint64_t stack = (uint64_t) kmalloc_pa(0x200000,0x200000);
	uint64_t vaddr = find_first_available_page();
	if(vaddr == -1){
		panic("Error: Out of memory!");
	}

	map_page(get_physical_addr(stack),vaddr,0b111); // User, write, present
	uint64_t args_kmem = (uint64_t) kmalloc_pa(0x200000,0x200000);
    uint64_t args_page = find_first_available_page();
	if(args_page == -1){
		panic("Error: Out of memory!");
	}
	map_page(get_physical_addr(args_kmem),args_page,0b111); // User, write, present
	
	set_page_directory(curr_dir);
	
	uint8_t* dst = (uint8_t*) args_kmem;
	uint8_t* arg_src = (uint8_t*) argv;
	uint8_t* env_src = (uint8_t*) envp;
	
	char** dst_argv = (char**) dst;

	uint64_t start = sizeof(char*)*argc+sizeof(char*)*envc+2;
	if(argv != 0x0){
		for(uint64_t i = 0; i < argc; i++){
			char* str = argv[i];
			uint64_t count = strlen(str);
			memcpy(&dst[start],str,count);
			dst_argv[i] = args_page+start;
			start += count+1; // Leave space for null terminator
		}
	}
	start++;
	uint64_t env_start = start;
	if(envp != 0x0){
		for(uint64_t i = 0; i < envc; i++){
			char* str = envp[i];
			uint64_t count = strlen(str);
			memcpy(&dst[start],str,count);
			dst_argv[i+env_start] = args_page+start;
			start += count+1; // Leave space for null terminator
		}
	}

    processes[slot].page_table = loaded_data;
	if(processes[slot].parent == 0){
		processes[slot].root_directory = get_true_root();
		processes[slot].current_directory = processes[slot].root_directory;
		processes[slot].uid = 0;
		processes[slot].gid = 0;
	}
	processes[slot].regs.userrsp = vaddr+0x200000; // Set stack pointer, note that stack grows down
    processes[slot].regs.rip = entry;
    processes[slot].regs.cs = 0x1b;
    processes[slot].shadow_stack = ((uint64_t) kmalloc_p(STACK_SIZE)) + STACK_SIZE; // Set kernel shadow stack pointer, note that stack grows down
    processes[slot].regs.ds = 0x23;
    processes[slot].regs.userss = 0x23;
    processes[slot].regs.rflags = 0b1000000010;
	
	// Setup argv,argc,envp,envc for program
	processes[slot].regs.rdi = (uint64_t) argc;
	processes[slot].regs.rsi = args_page;
	processes[slot].regs.rdx = (uint64_t) envc;
	processes[slot].regs.rcx = args_page + env_start;
	
    processes[slot].status = PROCESS_READY;
    processes[slot].count = MAX_DESCRIPTOR_COUNT;
	
	if(interrupts)
	    asm volatile("sti");
}
void kill_process(uint64_t code){
	processes[code].status = PROCESS_DEAD;
}

