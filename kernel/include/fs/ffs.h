#pragma once

#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <mem/vmm.h>
#include <utils.h>
#include <log.h>

struct parameter_block{
	char signature[4];
	uint32_t flags;
	uint64_t sectors;
	uint64_t chain_start_sector;
	uint64_t first_data_sector;
	char reserved[480];
}__attribute__((packed));
typedef struct parameter_block parameter_block_t;

void init_ffs();
