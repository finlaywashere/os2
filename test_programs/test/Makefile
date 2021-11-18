NAME = testprograms-$(OSNAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin

AS = nasm
CC = x86_64-elf-fos-gcc

OUTPUT = ../sysroot/

OBJS = src/test.o

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS)

AFLAGS = -felf64 -g -F dwarf
CFLAGS = -g -F dwarf -mno-sse -mno-sse2 -fno-tree-vectorize

.PHONY: testprograms link clean setup copy
.SUFFIXES: .o .c .asm
testprograms: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $^ -o $@
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
copy:
	for file in $(OBJS) ; do \
		cp $$file $(OUTPUT) ; \
	done
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
	rm -f $(OUTPUT)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
