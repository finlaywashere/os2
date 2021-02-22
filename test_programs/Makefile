NAME = testprograms-$(OSNAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin

AS = nasm
CC = x86_64-elf-fos-gcc
FFS2UTILS = ../../FFS2/ffs2_utils.o

OUTPUT = ../ffs.iso


OBJS = src/test.o \
src/test2.o \
src/test_c.o

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS)

AFLAGS = -felf64 -g -F dwarf
CFLAGS = -g -F dwarf

.PHONY: testprograms link clean setup iso
.SUFFIXES: .o .c .asm
testprograms: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
iso:
	$(FFS2UTILS) mkfs $(OUTPUT) 64
	for file in $(OBJS) ; do \
		$(FFS2UTILS) copyfile $(OUTPUT) $$file $$(basename $$file) ; \
	done
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
	rm -f $(OUTPUT)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
	@mkdir -p $(INCLUDEDIR)
