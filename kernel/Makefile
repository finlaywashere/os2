OSNAME = $(NAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin
ARCHDIR = $(SRCDIR)/arch/$(ARCH)

AS = nasm
CC = x86_64-elf-gcc

include $(ARCHDIR)/Makefile

OBJS = $(ARCH_OBJS) \
src/kernel.o \
src/mem/pmm.o \
src/utils.o

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
kernel: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
link:
	$(CC) -o $(BUILDDIR)/kernel.elf $(CFLAGS) $(LINK_LIST)
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(ARCHDIR)
	@mkdir -p $(INCLUDEDIR)
