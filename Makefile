NAME = OS2

QEMU = qemu-system-x86_64

QEMUFLAGS = --no-reboot \
-no-shutdown \
-m size=1024 \
-drive id=disk,file=disk.iso,if=none \
-device ich9-ahci,id=ahci  \
-device ide-hd,drive=disk,bus=ahci.0


build: install-headers
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) && $(MAKE) link
	@export NAME=$(NAME) && cd libc/ && $(MAKE) && $(MAKE) link && $(MAKE) install
	@export NAME=$(NAME) && cd bootloader/ && $(MAKE) && $(MAKE) iso
	@export NAME=$(NAME) && cd test_programs/ && $(MAKE)
	cp bootloader/bootloader.iso disk.iso
	dd if=kernel/bin/kernel.elf of=disk.iso bs=512 seek=33 count=94
	dd if=/dev/zero of=disk.iso bs=512 seek=130 count=81920
	./mkdisk.sh
	sudo mount -o loop,offset=66560,umask=000 disk.iso disk/
	@export NAME=$(NAME) && cd test_programs/ && $(MAKE) iso
	sudo umount disk/

clean:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) clean
	@export NAME=$(NAME) && cd libc/ && $(MAKE) clean
	@export NAME=$(NAME) && cd bootloader/ && $(MAKE) clean
	@export NAME=$(NAME) && cd test_programs/ && $(MAKE) clean
	@rm -f disk.iso
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) setup
	@export NAME=$(NAME) && cd libc/ && $(MAKE) setup
	@export NAME=$(NAME) && cd bootloader/ && $(MAKE) setup
	@export NAME=$(NAME) && cd test_programs/ && $(MAKE) setup
install-headers:
	rm -rf sysroot/
	mkdir -p sysroot/usr/include/
	cp -r libc/include/* sysroot/usr/include/
qemu: build
	$(QEMU) $(QEMUFLAGS) -monitor stdio --enable-kvm
gdb:
	$(QEMU) $(QEMUFLAGS) -monitor stdio -s -S --enable-kvm
debug: build
	$(QEMU) $(QEMUFLAGS) -d int
gdbd: build
	$(QEMU) $(QEMUFLAGS) -d int -s -S
