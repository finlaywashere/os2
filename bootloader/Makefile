OSNAME = bootloader-$(NAME)

STAGE1ARCH = i686
STAGE2ARCH = x86_64

SRCDIR = src
BUILDDIR = bin
STAGE1ARCHDIR = $(SRCDIR)/arch/$(STAGE1ARCH)
STAGE2ARCHDIR = $(SRCDIR)/arch/$(STAGE2ARCH)
AS = nasm
CC = x86_64-elf-fos-gcc

QEMU = qemu-system-x86_64
ISO = bootloader.iso

include $(STAGE1ARCHDIR)/Makefile
include $(STAGE2ARCHDIR)/Makefile

OBJS = $(STAGE1ARCH_OBJS) \
$(STAGE2ARCH_OBJS)

LINK_LIST=\
$(LDFLAGS) \
$(STAGE2ARCH_OBJS) \
$(LIBS)

INCLUDEDIR=include

CFLAGS= \
-O2 			       \
-g 			       \
-F dwarf 		       \
-fno-pic	               \
-z max-page-size=0x1000        \
-mno-sse                       \
-mno-sse2                      \
-mno-mmx                       \
-mno-80387                     \
-mno-red-zone                  \
-mcmodel=kernel                \
-ffreestanding                 \
-fno-stack-protector           \
-fno-omit-frame-pointer        \
-Wall			       \
-I$(INCLUDEDIR)

ABINFLAGS = -f bin
AFLAGS = -felf64
LDFLAGS = -T $(STAGE2ARCHDIR)/linker.ld -shared -Bsymbolic -nostdlib

.PHONY: link clean setup iso qemu debug gdb
.SUFFIXES: .o .c .asm .asmb
bootloader: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@
.asmb.o:
	$(AS) $(ABINFLAGS) $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
link:
	$(CC) -o $(BUILDDIR)/bootloader.elf $(CFLAGS) $(LINK_LIST)
	objcopy -O binary -j .text -j .rodata $(BUILDDIR)/bootloader.elf $(BUILDDIR)/bootloader.bin
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
	rm -f bootloader.iso
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(STAGE1ARCHDIR)
	@mkdir -p $(STAGE2ARCHDIR)
	@mkdir -p $(INCLUDEDIR)
iso:
	rm -f $(ISO)
	dd if=src/arch/i686/bootloader.o of=$(ISO) bs=512 count=1 seek=0
	dd if=src/arch/x86_64/stage2.o of=$(ISO) bs=512 seek=1 count=4
qemu:
	$(QEMU) --no-reboot -no-shutdown --hda $(ISO) -monitor stdio
debug:
	$(QEMU) --no-reboot -no-shutdown --hda $(ISO) -d int
gdb:
	$(QEMU) --no-reboot -no-shutdown --hda $(ISO) -d int -s -S
