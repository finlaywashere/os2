NAME = OS2

QEMU = qemu-system-x86_64

QEMUFLAGS = --no-reboot -no-shutdown -drive file=disk.iso,id=drive0,if=ide

build:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) && $(MAKE) link
	@export NAME=$(NAME) && cd libc/ && $(MAKE)
	@export NAME=$(NAME) && cd bootloader && $(MAKE)
	export NAME=$(NAME) && cd bootloader && $(MAKE) iso
	@dd if=/dev/zero of=fs.iso bs=1024 count=64K # 64MiB root partition
	mkfs.echfs fs.iso 512 25
	@mkdir -p isodir/
	@echfs-fuse fs.iso isodir/
	cp kernel/bin/kernel.elf isodir/kernel.elf
	@umount isodir/
	cp bootloader/bin/stage1.o disk.iso
	dd if=bootloader/bin/stage2.o of=disk.iso bs=1 seek=512
	@dd if=fs.iso of=disk.iso bs=512 seek=2048 count=128K
	@echo 'start=2048, size=129024, type=c, bootable' | sfdisk disk.iso 
clean:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) clean
	@export NAME=$(NAME) && cd libc/ && $(MAKE) clean
	@export NAME=$(NAME) && cd bootloader && $(MAKE) clean
	@rm -f disk.iso
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) setup
	@export NAME=$(NAME) && cd libc/ && $(MAKE) setup
	@export NAME=$(NAME) && cd bootloader && $(MAKE) setup
qemu:
	$(QEMU) $(QEMUFLAGS) -monitor stdio
debug:
	$(QEMU) $(QEMUFLAGS) -d int
gdb:
	$(QEMU) $(QEMUFLAGS) -d int -s -S
gdbm:
	$(QEMU) $(QEMUFLAGS) -monitor stdio -s -S
