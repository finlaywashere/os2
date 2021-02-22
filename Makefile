NAME = OS2

QEMU = qemu-system-x86_64

QEMUFLAGS = --no-reboot -no-shutdown -drive file=disk.iso,id=drive0,if=ide -drive file=ffs.iso,id=drive1,if=ide

build: install-headers
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) && $(MAKE) link
	@export NAME=$(NAME) && cd libc/ && $(MAKE) && $(MAKE) link && $(MAKE) install
	@export NAME=$(NAME) && cd bootloader/ && $(MAKE) && $(MAKE) iso
	@export NAME=$(NAME) && cd test_programs/ && $(MAKE) && $(MAKE) iso
	cp bootloader/bootloader.iso disk.iso
	dd if=kernel/bin/kernel.elf of=disk.iso bs=512 seek=33 count=94
clean:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) clean
	@export NAME=$(NAME) && cd libc/ && $(MAKE) clean
	@export NAME=$(NAME) && cd bootloader && $(MAKE) clean
	@rm -f disk.iso
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) setup
	@export NAME=$(NAME) && cd libc/ && $(MAKE) setup
	@export NAME=$(NAME) && cd bootloader && $(MAKE) setup
install-headers:
	rm -rf sysroot/
	mkdir -p sysroot/usr/include/
	cp -r libc/include/* sysroot/usr/include/
qemu:
	$(QEMU) $(QEMUFLAGS) -monitor stdio
debug:
	$(QEMU) $(QEMUFLAGS) -d int
gdb:
	$(QEMU) $(QEMUFLAGS) -d int -s -S
gdbm:
	$(QEMU) $(QEMUFLAGS) -monitor stdio -s -S
