#include <process.h>

uint64_t curr_max_process = 0;
uint64_t curr_process = 0;

process_t* processes;

void init_processes(){
	processes = (process_t*) kmalloc_p(sizeof(process_t)*MAX_PROCESS_COUNT); // Max of # of processes
}
process_t* get_process(){
	return &processes[curr_process];
}
uint64_t get_curr_process(){
	return curr_process;
}
void schedule(registers_t* regs){
	if(curr_max_process == 0)
		return;
	memcpy(regs,&processes[curr_process].regs,sizeof(registers_t));
	processes[curr_process].status = PROCESS_READY;
	curr_process++;
	int looped = 0;
	while(processes[curr_process].status != PROCESS_READY){
		curr_process++;
		if(curr_process >= MAX_PROCESS_COUNT && looped == 0){
			curr_process = 1;
		}else if(curr_process >= MAX_PROCESS_COUNT){
			curr_process = 0;
			break;
		}
	}
	processes[curr_process].status = PROCESS_RUNNING;
	tss_set_rsp(processes[curr_process].shadow_stack);
	memcpy(&processes[curr_process].regs,regs,sizeof(registers_t));
	set_page_directory(processes[curr_process].page_table);
}
uint64_t read(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
        uint64_t seek = descriptor->buffer_seek;
        uint8_t* src_buffer = (uint8_t*) (((uint64_t) descriptor->buffer) + seek);
        uint64_t src_size = descriptor->buffer_size;
        if(seek >= size)
                return 0;
        uint64_t count = src_size - seek;
        if(count > size)
                count = size;
        descriptor->buffer_seek = seek+count;
        memcpy(src_buffer,buffer,count);
	return count;
}
uint64_t write(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	uint64_t dst_seek = descriptor->buffer_seek;
	uint8_t* dst_buffer = (uint8_t*) (((uint64_t) descriptor->buffer) + dst_seek);
	uint64_t dst_size = descriptor->buffer_size;
	if(dst_seek >= size)
		return 0;
	uint64_t count = dst_size - dst_seek;
	if(count > size)
		count = size;
	descriptor->buffer_seek = dst_seek+count;
	memcpy(buffer,dst_buffer,count);
	return count;
}
uint64_t write_screen(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	tty_putchars(0, (char*) buffer, size);
	return size;
}
uint64_t read_keyboard(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	//TODO: Implement this
	return 0;
}
uint8_t configure_descriptors(uint64_t pid, uint64_t parent){
	if(parent == 0){
		processes[pid].descriptors[0].write = &write_screen;
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
		processes[parent].descriptors[parent_descriptor_slot].read = &read;
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
		processes[parent].descriptors[parent_descriptor_slot+1].id = pid << 32 | 1;
	}
	return 0;
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
	memcpy(regs,&processes[parent].regs,sizeof(registers_t));
	uint64_t child = find_slot();
	if(child == 0)
		return 0;
	memcpy(&processes[parent],&processes[child],sizeof(process_t));
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
uint64_t create_process(page_table_t* loaded_data, uint64_t entry, uint64_t parent){
	uint64_t slot = find_slot();
	if(slot == 0)
		return 0;
	create_process_pid(slot,loaded_data,entry,parent);
	curr_max_process++;
	return slot;
}
void create_process_pid(uint64_t pid, page_table_t* loaded_data, uint64_t entry, uint64_t parent){
	asm volatile("cli");
	uint64_t slot = pid;
	
	create_process_pid_nodesc(pid,loaded_data,entry);
	processes[slot].parent = parent;

	configure_descriptors(slot,parent);
	
	asm volatile("sti");
	return slot;
}
void create_process_pid_nodesc(uint64_t pid, page_table_t* loaded_data, uint64_t entry){
	asm volatile("cli");
    uint64_t slot = pid;
	memset(&processes[slot].regs,0,sizeof(registers_t));
    processes[slot].page_table = loaded_data;
    processes[slot].regs.rip = entry;
    processes[slot].regs.cs = 0x1b;
    processes[slot].shadow_stack = (uint64_t) kmalloc_p(STACK_SIZE);
    processes[slot].regs.ds = 0x23;
    processes[slot].regs.userss = 0x23;
    processes[slot].regs.rflags = 0b1000000010;
    processes[slot].status = PROCESS_READY;
    processes[slot].count = MAX_DESCRIPTOR_COUNT;

    asm volatile("sti");
}
void kill_process(uint64_t code){
	processes[code].status = PROCESS_DEAD;
}

