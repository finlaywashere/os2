NAME = libc-$(OSNAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin
ARCHDIR = $(SRCDIR)/arch/$(ARCH)

AS = nasm
CC = x86_64-elf-fos-gcc
AR = x86_64-elf-fos-ar

include $(ARCHDIR)/Makefile

OBJS = $(ARCH_OBJS) \
src/libc.o \
src/syscall.o \
src/stdio.o

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS)


INCLUDEDIR=include

CFLAGS= \
-O2 \
-g \
-F dwarf \
-mno-sse \
-mno-sse2 \
-fno-tree-vectorize \
-Wextra \
-Wall \
-mno-red-zone \
-Wall			       \
-I$(INCLUDEDIR)

AFLAGS = -felf64 -g -F dwarf
LDFLAGS = -T $(ARCHDIR)/linker.ld -shared -Bsymbolic -nostdlib

.PHONY: libc link clean setup install
.SUFFIXES: .o .c .asm
libc: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
link:
	$(AR) rcs bin/libc.a $(OBJS)
	cp $(ARCHDIR)/crt*.o bin/
install:
	mkdir -p ../sysroot/usr/lib/
	cp bin/* ../sysroot/usr/lib/
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(ARCHDIR)
	@mkdir -p $(INCLUDEDIR)
