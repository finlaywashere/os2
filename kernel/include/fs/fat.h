#ifndef FAT_H
#define FAT_H

#include <stdint.h>
#include <stddef.h>
#include <fs/fs.h>
#include <arch/x86_64/driver/disk/disk.h>
#include <mem/vmm.h>
#include <utils.h>
#include <log.h>

#define READ_ONLY 0x1
#define HIDDEN 0x2
#define SYSTEM 0x4
#define VOLUME_ID 0x8
#define DIRECTORY 0x10
#define ARCHIVE 0x20

#define LFN 0xF

struct bpb {
	char loop[3];
	char oem[8];
	uint16_t bytes_per_sector;
	uint8_t sectors_per_cluster;
	uint16_t reserved_sectors;
	uint8_t num_fats;
	uint16_t dir_entries;
	uint16_t total_sectors;
	uint8_t media_type;
	uint16_t sectors_per_fat;
	uint16_t sectors_per_track;
	uint16_t num_heads;
	uint32_t partition_lba;
	uint32_t large_sector_count;
}__attribute__((packed));
typedef struct bpb bpb_t;

struct ebpb {
	uint32_t sectors_per_fat;
	uint16_t flags;
	uint16_t version;
	uint32_t root_cluster;
	uint16_t fsinfo_sector;
	uint16_t backup_boot_sector;
	char reserved[12];
	uint8_t drive_number;
	uint8_t reserved_2;
	uint8_t signature;
	uint32_t volume_id;
	char label[11];
	char system_identifier[8];
}__attribute__((packed));
typedef struct ebpb ebpb_t;

struct fat_bpb{
	bpb_t bpb;
	ebpb_t ebpb;
}__attribute__((packed));
typedef struct fat_bpb fat_bpb_t;

struct fsinfo{
	uint32_t lead_signature;
	char reserved[480];
	uint32_t signature;
	uint32_t free_cluster_count;
	uint32_t first_available_cluster;
	char reserved_2[12];
	uint32_t trail_signature;
}__attribute__((packed));
typedef struct fsinfo fsinfo_t;

struct fat_entry{
	uint32_t value : 28;
	uint8_t reserved : 4;
}__attribute__((packed));
typedef struct fat_entry fat_entry_t;

struct fat_dir_entry{
	char name[8];
	char ext[3];
	uint8_t attributes;
	uint8_t reserved;
	uint8_t creation_time_ts;
	uint16_t creation_time;
	uint16_t creation_date;
	uint16_t access_date;
	uint16_t cluster_high;
	uint16_t modification_time;
	uint16_t modification_date;
	uint16_t cluster_low;
	uint32_t size;
}__attribute__((packed));
typedef struct fat_dir_entry fat_dir_entry_t;

struct fat_dir_entry_lfn{
	uint8_t order;
	uint16_t chars_1[5];
	uint8_t attribute;
	uint8_t type;
	uint8_t short_checksum;
	uint16_t chars_2[6];
	uint16_t zero;
	uint16_t chars_3[2];
}__attribute__((packed));
typedef struct fat_dir_entry_lfn fat_dir_entry_lfn_t;

void init_fat();
uint8_t fat_write_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer);
uint8_t fat_read_file(fs_node_t* file, uint64_t offset, uint64_t length, uint8_t* buffer);
uint8_t fat_read_dir(fs_node_t* dir, fs_node_t* buffer);
uint8_t fat_write_dir(fs_node_t* file, uint64_t offset, uint64_t length, fs_node_t* buffer);
uint64_t fat_dir_entries(fs_node_t* file);
uint8_t fat_create_file(fs_node_t* dir, char* name, uint16_t flags, uint16_t type);


#endif
