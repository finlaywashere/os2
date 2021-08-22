#ifndef GPT_H
#define GPT_H

#include <stdint.h>
#include <stddef.h>

struct partition_table_header{
	char signature[8];
	uint32_t revision;
	uint32_t header_size;
	uint32_t header_checksum;
	uint32_t reserved;
	uint64_t header_lba;
	uint64_t alt_header_lba;
	uint64_t first_usable_block;
	uint64_t last_usable_block;
	char guid[16];
	uint64_t guid_entry_lba;
	uint32_t partition_entries;
	uint32_t partition_entry_size;
	uint32_t partition_checksum;
}__attribute__((packed));
typedef struct partition_table_header partition_table_header_t;

struct partition_entry{
	char type_guid[16];
	char part_guid[16];
	uint64_t start_lba;
	uint64_t end_lba;
	uint64_t attributes;
	char name[72];
}__attribute__((packed));

#endif
