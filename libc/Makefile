NAME = libc-$(OSNAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin
ARCHDIR = $(SRCDIR)/arch/$(ARCH)

AS = nasm
CC = x86_64-elf-fos-gcc
AR = x86_64-elf-fos-ar

include $(ARCHDIR)/Makefile

OBJS = $(ARCH_OBJS)

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS)


INCLUDEDIR=include

CFLAGS= \
-O2 \
-g \
-F dwarf \
-ffreestanding \
-Wextra \
-Wall \
-mno-red-zone \
-Wall			       \
-I$(INCLUDEDIR)

AFLAGS = -felf64 -g -F dwarf
LDFLAGS = -T $(ARCHDIR)/linker.ld -shared -Bsymbolic -nostdlib

.PHONY: libc link clean setup
.SUFFIXES: .o .c .asm
libc: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) -c $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
link:
	$(AR) rcs bin/libc.a $(OBJS)
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(ARCHDIR)
	@mkdir -p $(INCLUDEDIR)
