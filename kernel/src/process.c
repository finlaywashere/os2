#include <process.h>

uint64_t curr_max_process = 0;
uint64_t curr_process = 0;

process_t* processes;

void init_processes(){
	processes = (process_t*) kmalloc_p(sizeof(process_t)*MAX_PROCESS_COUNT); // Max of # of processes
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
uint64_t create_process(page_table_t* loaded_data, uint64_t entry){
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
	
	curr_max_process++;
		
	asm volatile("sti");
	return slot;
}
void kill_process(uint64_t code){
	processes[code].status = PROCESS_DEAD;
}

