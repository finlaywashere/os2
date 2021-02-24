#include <process.h>

uint64_t curr_max_process = 0;
uint64_t curr_process = 0;

process_t* processes;

fs_node_t* files;

void init_processes(){
	processes = (process_t*) kmalloc_p(sizeof(process_t)*MAX_PROCESS_COUNT); // Max of MAX_PROCESS_COUNT of processes
	files = (fs_node_t*) kmalloc_p(sizeof(fs_node_t)*MAX_FILE_COUNT); // Max MAX_FILE_COUNT of open files
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
uint64_t read_file(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	if(descriptor == 0)
        return 0;
	uint64_t offset = descriptor->buffer_seek;
	
	fs_node_t* file = &files[descriptor->id]; // Grab file from the descriptor's internal id
	uint64_t read = file->read_file(file,offset,size,buffer);
	
	descriptor->buffer_seek += read;
	
	return read;
}
uint64_t write_file(descriptor_t *descriptor, uint8_t* buffer, uint64_t size){
	if(descriptor == 0)
        return 0;
	uint64_t offset = descriptor->buffer_seek;

    fs_node_t* file = &files[descriptor->id]; // Grab file from the descriptor's internal id
    file->write_file(file,offset,size,buffer);
    
	descriptor->buffer_size = file->length;
	
	return size;
}
fs_node_t* get_descriptor_file(uint64_t id){
    descriptor_t desc = processes[curr_process].descriptors[id];
    if(desc.read != &read_file)
        return 0;
    return &files[desc.id];
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
	memcpy(src_buffer,buffer,count);
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
		if(files[i].name[0] == 0){
			file_slot = i;
			break;
		}
	}
	if(file_slot == 0)
		return 0;
	if(!(mode & MODE_WRITE)){
		get_file(name,&files[file_slot]); // Read file into file slot
		if(files[file_slot].name[0] == 0)
			return 0; // File not found
	}else{
		create_file(name,&files[file_slot]);
	}
	
	processes[curr_process].descriptors[desc_slot].id = file_slot;
	processes[curr_process].descriptors[desc_slot].buffer_size = files[file_slot].length;
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
uint64_t create_process(page_table_t* loaded_data, uint64_t entry, uint64_t parent, char* argv[], int argc, char* envp[], int envc){
	uint64_t slot = find_slot();
	if(slot == 0)
		return 0;
	create_process_pid(slot,loaded_data,entry,parent,argv,argc,envp,envc);
	curr_max_process++;
	return slot;
}
void create_process_pid(uint64_t pid, page_table_t* loaded_data, uint64_t entry, uint64_t parent, char* argv[], int argc, char* envp[], int envc){
	asm volatile("cli");
	uint64_t slot = pid;
	
	create_process_pid_nodesc(pid,loaded_data,entry,argv,argc,envp,envc);
	processes[slot].parent = parent;

	configure_descriptors(slot,parent);
	
	asm volatile("sti");
	return slot;
}
void create_process_pid_nodesc(uint64_t pid, page_table_t* loaded_data, uint64_t entry, char* argv[], int argc, char* envp[], int envc){
	asm volatile("cli");
    uint64_t slot = pid;
	memset(&processes[slot].regs,0,sizeof(registers_t));
	
	page_table_t* curr_dir = get_curr_page_directory();
	set_page_directory(loaded_data);

	// Setup process' stack
	// 2MiB stack for convenience
	uint64_t stack = (uint64_t) kmalloc_pa(0x200000,0x200000);
	uint64_t vaddr = find_first_available_page();
	uint64_t args_kmem = (uint64_t) kmalloc_pa(0x200000,0x200000);
	uint64_t args_page = find_first_available_page();
	if(vaddr == -1 || args_page == -1){
		panic("Error: Out of memory!");
	}
	map_page(get_physical_addr(stack),vaddr,0b111); // User, write, present
	map_page(get_physical_addr(args_kmem),args_page,0b111); // User, write, present
	
	set_page_directory(curr_dir);
	
	uint8_t* dst = (uint8_t*) args_kmem;
	uint8_t* arg_src = (uint8_t*) argv;
	uint8_t* env_src = (uint8_t*) envp;
	
	uint64_t env_start = (argc+1) * sizeof(void*);
	uint64_t len = (argc+envc+2)*sizeof(void*);
	
	for(int i = 0; i < len; i++){
		if(i < env_start-sizeof(void*)){
			// Args
			dst[i] = arg_src[i];
		}else if(i >= env_start-sizeof(void*) && i < env_start){
			// Null
			dst[i] = 0x0;
		}else if(i < len-sizeof(void*)){
			// Envp
			dst[i] = env_src[i];
		}else{
			// Null
			dst[i] = 0x0;
		}
	}
	
    processes[slot].page_table = loaded_data;
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

    asm volatile("sti");
}
void kill_process(uint64_t code){
	processes[code].status = PROCESS_DEAD;
}

