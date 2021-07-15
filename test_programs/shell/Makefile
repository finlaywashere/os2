NAME = shell-$(OSNAME)

ARCH = x86_64

SRCDIR = src
BUILDDIR = bin

AS = nasm
CC = x86_64-elf-fos-gcc

OUTPUT = ../sysroot/

OBJS = src/shell.o

DATA_FILES = 

LINK_LIST=\
$(LDFLAGS) \
$(OBJS) \
$(LIBS)

AFLAGS = -felf64 -g -F dwarf
CFLAGS = -g -F dwarf -mno-sse -mno-sse2 -fno-tree-vectorize
LIBS = -lc

.PHONY: shell clean setup copy
.SUFFIXES: .o .c .asm
shell: $(OBJS)

.c.o:
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)
.asm.o:
	$(AS) $(AFLAGS) $^ -o $@
copy:
	for file in $(DATA_FILES) ; do \
        cp $$file $(OUTPUT) ; \
    done
	for file in $(OBJS) ; do \
		cp $$file $(OUTPUT) ; \
	done
clean:
	rm -rf $(BUILDDIR)
	rm -f $(OBJS)
setup:
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(SRCDIR)
