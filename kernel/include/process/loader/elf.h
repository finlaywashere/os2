#ifndef ELF_H
#define ELF_H

#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>
#include <arch/x86_64/paging.h>
#include <utils.h>
#include <mem/vmm.h>
#include <arch/x86_64/idt.h>
#include <process/safety.h>

#define ELF_SEGMENT_NULL 0
#define ELF_SEGMENT_LOAD 1
#define ELF_SEGMENT_DYNAMIC 2
#define ELF_SEGMENT_INTERPRETER 3
#define ELF_SEGMENT_NOTE 4

#define ELF_FLAG_EXEC 1
#define ELF_FLAG_WRITE 2
#define ELF_FLAG_READ 4

struct elf_header{
	char sig[4];
	uint8_t bits;
	uint8_t endian;
	uint8_t header_version;
	uint8_t abi;
	uint64_t reserved;
	uint16_t type;
	uint16_t instruction_set;
	uint32_t elf_version;
	uint64_t entry_position;
	uint64_t header_table_position;
	uint64_t section_table_position;
	uint32_t flags;
	uint16_t header_size;
	uint16_t program_header_entry_size;
	uint16_t program_header_num_entries;
	uint16_t section_header_entry_size;
	uint16_t section_header_num_entries;
	uint16_t section_header_name_index;
}__attribute__((packed));

typedef struct elf_header elf_header_t;

struct elf_program_header{
	uint32_t type;
	uint32_t flags;
	uint64_t file_data_offset;
	uint64_t memory_data_offset;
	uint64_t reserved;
	uint64_t file_size;
	uint64_t memory_size;
	uint64_t alignment;
}__attribute__((packed));

typedef struct elf_program_header elf_program_header_t;

uint64_t load_elf(fs_node_t* file, page_table_t* dst); // Loads an ELF file from disk and returns its entry point

#endif
