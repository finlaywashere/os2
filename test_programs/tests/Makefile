NAME = testprograms-$(OSNAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin

AS = nasm
CC = x86_64-elf-fos-gcc
FFS2UTILS = ../../../FFS2/ffs2_utils.o

OUTPUT = ../../ffs.iso


OBJS = src/test.o \
src/test2.o \
src/test_c.o

DATA_FILES = data/test.txt

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS)

AFLAGS = -felf64 -g -F dwarf
CFLAGS = -g -F dwarf -mno-sse -mno-sse2 -fno-tree-vectorize

.PHONY: testprograms link clean setup iso
.SUFFIXES: .o .c .asm
testprograms: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
.txt:
iso:
	$(FFS2UTILS) mkfs $(OUTPUT) 64
	for file in $(DATA_FILES) ; do \
        $(FFS2UTILS) copyfile $(OUTPUT) $$file $$(basename $$file) ; \
    done
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
