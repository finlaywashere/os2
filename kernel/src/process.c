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
void schedule(registers_t* regs){
	if(curr_max_process == 0)
		return;
	memcpy(regs,&processes[curr_process].regs,sizeof(registers_t));
	curr_process++;
	if(curr_process >= curr_max_process+1){
		curr_process = 1;
	}
	tss_set_rsp(processes[curr_process].shadow_stack);
	memcpy(&processes[curr_process].regs,regs,sizeof(registers_t));
	set_page_directory(processes[curr_process].page_table);
}
uint64_t read(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
        asm volatile("cli");
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
        asm volatile("sti");
	return count;
}
uint64_t write(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	descriptor_t *pdesc = &processes[descriptor->parent_pid].descriptors[descriptor->did];
	asm volatile("cli");
	uint64_t dst_seek = pdesc->buffer_seek;
	uint8_t* dst_buffer = (uint8_t*) (((uint64_t) pdesc->buffer) + dst_seek);
	uint64_t dst_size = pdesc->buffer_size;
	if(dst_seek >= size)
		return 0;
	uint64_t count = dst_size - dst_seek;
	if(count > size)
		count = size;
	pdesc->buffer_seek = dst_seek+count;
	memcpy(buffer,dst_buffer,count);
	asm volatile("sti");
	return count;
}
uint64_t write_screen(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	tty_putchars(0, (char*) buffer, size);
	return size;
}

uint64_t create_process(page_table_t* loaded_data, uint64_t entry, uint64_t parent){
	asm volatile("cli");
	uint64_t slot = 0;
	for(uint64_t i = 1; i < MAX_PROCESS_COUNT; i++){
		if(processes[i].status == PROCESS_DEAD){
			slot = i;
			break;
		}
	}
	if(slot == 0)
		return -1;
	processes[slot].page_table = loaded_data;
	processes[slot].regs.rip = entry;
	processes[slot].regs.cs = 0x1b;
	processes[slot].shadow_stack = (uint64_t) kmalloc_p(STACK_SIZE);
	processes[slot].regs.ds = 0x23;
	processes[slot].regs.userss = 0x23;
	processes[slot].regs.rflags = 0b1000000010;
	processes[slot].status = PROCESS_READY;
	processes[slot].parent = parent;
	processes[slot].count = MAX_DESCRIPTOR_COUNT;
	
	if(parent == 0){
		processes[slot].descriptors[0].write = &write_screen;
	}else{
		uint64_t parent_descriptor_slot = 0;
		for(uint64_t i = 2; i < processes[slot].count; i++){
			if(processes[parent].descriptors[i].flags != DESCRIPTOR_PRESENT){
				parent_descriptor_slot = i;
				break;
			}
		}
		if(parent_descriptor_slot == 0)
			return -2;
		// Lowest is always the child's stdout (descriptor 0 on child process)
		processes[parent].descriptors[parent_descriptor_slot].buffer = (uint8_t*) kmalloc_p(BUFFER_SIZE);
		processes[parent].descriptors[parent_descriptor_slot].buffer_size = BUFFER_SIZE;
		processes[parent].descriptors[parent_descriptor_slot].read = &read;
		processes[slot].descriptors[0].buffer = (uint8_t*) kmalloc_p(BUFFER_SIZE);
		processes[slot].descriptors[0].buffer_size = BUFFER_SIZE;
		processes[slot].descriptors[0].write = &write;
		processes[slot].descriptors[0].parent_pid = parent;
		processes[slot].descriptors[0].did = parent_descriptor_slot;
	}
	
	curr_max_process++;
		
	asm volatile("sti");
	return slot;
}
void kill_process(uint64_t code){
	processes[code].status = PROCESS_DEAD;
}

