#include <arch/x86_64/driver/disk/disk.h>

disk_t* disks;
uint8_t disk_count;

void init_disks(){
	disks = (disk_t*) kmalloc_p(sizeof(disk_t) * 256); // Max of 256 disks
	init_ide();
}
uint8_t get_disk_count(){
	return disk_count;
}
void register_disk(disk_t disk){
	disks[disk_count] = disk;
	disk_count++;
}
disk_t* get_disk(uint8_t index){
	return &disks[index];
}
uint8_t read_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer){
	if(id > disk_count)
		return 1;
	disk_t* disk = &disks[id];
	if(!(disk->flags & 1))
		return 2;
	return disk->read_disk(disk,lba,count,buffer);
}
uint8_t write_disk(uint8_t id, uint64_t lba, uint64_t count, uint8_t* buffer){
	if(id > disk_count)
                return 1;
        disk_t* disk = &disks[id];
        if(!(disk->flags & 1))
                return 2;
        return disk->write_disk(disk,lba,count,buffer);
}
