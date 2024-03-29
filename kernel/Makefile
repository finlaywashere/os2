OSNAME = $(NAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin
ARCHDIR = $(SRCDIR)/arch/$(ARCH)

AS = nasm
CC = x86_64-elf-fos-gcc

include $(ARCHDIR)/Makefile

OBJS = $(ARCH_OBJS) \
src/kernel.o \
src/mem/pmm.o \
src/mem/vmm.o \
src/utils.o \
src/log.o \
src/fs/fs.o \
src/fs/ffs.o \
src/fs/devfs.o \
src/process/loader/elf.o \
src/process/process.o \
src/process/syscall.o \
src/process/safety.o \
src/process/signal.o \
src/fs/fat.o \
src/security/user.o

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
-fno-builtin				\
-fno-tree-loop-distribute-patterns \
-Wall			       \
-I$(INCLUDEDIR)

AFLAGS = -felf64 -g -F dwarf
LDFLAGS = -T $(ARCHDIR)/linker.ld -shared -Bsymbolic -nostdlib

.PHONY: link kernel clean setup
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
