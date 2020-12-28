NAME = OS2

QEMU = qemu-system-x86_64

build:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) && $(MAKE) link
	@export NAME=$(NAME) && cd libc/ && $(MAKE)
	@export NAME=$(NAME) && cd bootloader && $(MAKE)
clean:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) clean
	@export NAME=$(NAME) && cd libc/ && $(MAKE) clean
	@export NAME=$(NAME) && cd bootloader && $(MAKE) clean
	@rm -f disk.iso
setup:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) setup
	@export NAME=$(NAME) && cd libc/ && $(MAKE) setup
	@export NAME=$(NAME) && cd bootloader && $(MAKE) setup
iso:
	export NAME=$(NAME) && cd bootloader && $(MAKE) iso
	cp bootloader/bootloader.iso disk.iso
	dd if=kernel/bin/kernel.elf of=disk.iso bs=512 seek=33 count=32
qemu:
	$(QEMU) --no-reboot -no-shutdown --hda disk.iso -monitor stdio
debug:
	$(QEMU) --no-reboot -no-shutdown --hda disk.iso -d int
gdb:
	$(QEMU) --no-reboot -no-shutdown --hda disk.iso -d int -s -S
gdbm:
	$(QEMU) --no-reboot -no-shutdown --hda disk.iso -monitor stdio -s -S
