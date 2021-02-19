OSNAME = bootloader-$(NAME)

STAGE1ARCH = i686
STAGE2ARCH = x86_64

SRCDIR = src
BUILDDIR = bin
STAGE1ARCHDIR = $(SRCDIR)/arch/$(STAGE1ARCH)
STAGE2ARCHDIR = $(SRCDIR)/arch/$(STAGE2ARCH)
AS = nasm
CC = x86_64-elf-gcc

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

.SUFFIXES: .o .c .asm .asmb
bootloader: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@
.asmb.o:
	$(AS) $(ABINFLAGS) $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
iso:
	cp src/arch/i686/bootloader.o bin/stage1.o
	cp src/arch/x86_64/stage2.o bin/stage2.o
link:
	$(CC) -o $(BUILDDIR)/bootloader.elf $(CFLAGS) $(LINK_LIST)
	objcopy -O binary -j .text -j .rodata $(BUILDDIR)/bootloader.elf $(BUILDDIR)/bootloader.bin
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(STAGE1ARCHDIR)
	@mkdir -p $(STAGE2ARCHDIR)
	@mkdir -p $(INCLUDEDIR)
