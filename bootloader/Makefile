OSNAME = bootloader-$(NAME)

ARCH = i686

SRCDIR = src
BUILDDIR = bin
ARCHDIR = $(SRCDIR)/arch/$(ARCH)

AS = nasm
CC = i686-elf-gcc

include $(ARCHDIR)/Makefile

OBJS = $(ARCH_OBJS)

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
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

AFLAGS = -felf64 -g -F dwarf
LDFLAGS = -T $(ARCHDIR)/linker.ld -shared -Bsymbolic -nostdlib

.SUFFIXES: .o .c .asm
bootloader: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
link:
	$(CC) -o $(BUILDDIR)/bootloader.elf $(CFLAGS) $(LINK_LIST)
	objcopy -O binary -j .text -j .rodata $(BUILDDIR)/bootloader.elf $(BUILDDIR)/bootloader.bin
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(ARCHDIR)
	@mkdir -p $(INCLUDEDIR)
