NAME = OS2

build:
	
	@export NAME=$(NAME) && cd kernel/ && $(MAKE)
	@export NAME=$(NAME) && cd libc/ && $(MAKE)
	@export NAME=$(NAME) && cd bootloader && $(MAKE)
clean:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) clean
	@export NAME=$(NAME) && cd libc/ && $(MAKE) clean
	@export NAME=$(NAME) && cd bootloader && $(MAKE) clean
setup:
	@export NAME=$(NAME) && cd kernel/ && $(MAKE) setup
	@export NAME=$(NAME) && cd libc/ && $(MAKE) setup
	@export NAME=$(NAME) && cd bootloader && $(MAKE) setup

qemu:
	qemu-system-x86_64 -m 256M -L /usr/share/edk2-ovmf/x64/ -bios OVMF.fd -cdrom $(BUILDDIR)/$(OSNAME).img

