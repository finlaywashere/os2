#include <elf.h>

uint64_t load_elf(char* name, page_table_t* dst){
	fs_node_t* file = (fs_node_t*) kmalloc_p(sizeof(fs_node_t));
	get_file(name,file);
	uint8_t* buffer = (uint8_t*) kmalloc_p(file->length);
	file->read_file(file,0,file->length,buffer); // Read whole file to memory
	
	elf_header_t* header = (elf_header_t*) buffer;
	uint64_t header_addr = header->header_table_position + (uint64_t)buffer;
	
	page_table_t* curr_table = get_curr_page_directory();
        memcpy((uint8_t*) curr_table, (uint8_t*) dst, sizeof(page_table_t));
        disable_interrupts();
        set_page_directory(dst); // Set new page directory
	
	for(int i = 0; i < header->program_header_num_entries; i++){
		elf_program_header_t* p_header = (elf_program_header_t*) header_addr;
		uint8_t* section_buffer = (uint8_t*) kmalloc_pa(p_header->file_size/0x200000 + (p_header->file_size % 0x200000 > 0 ? 1 : 0),0x200000);
		memcpy((uint8_t*)(((uint64_t)buffer)+p_header->file_data_offset),section_buffer,p_header->file_size);
		map_pages(get_physical_addr((uint64_t) section_buffer), p_header->memory_data_offset, 0b111, p_header->memory_size);
		header_addr += header->program_header_entry_size;
	}
	
	set_page_directory(curr_table); // Restore old page directory
	enable_interrupts();
	return header->entry_position;
}
