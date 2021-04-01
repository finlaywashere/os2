NAME = testprograms-$(OSNAME)

FFS2UTILS = ../../FFS2/ffs2_utils.o
OUTPUT = ../ffs.iso

.PHONY: clean setup iso

build:
	@export NAME=$(NAME) && cd test/ && $(MAKE) && $(MAKE) copy
	@export NAME=$(NAME) && cd shell/ && $(MAKE) && $(MAKE) copy
iso:
	$(FFS2UTILS) mkfs $(OUTPUT) 64
	for file in sysroot/* ; do \
        $(FFS2UTILS) copyfile $(OUTPUT) $$file $$(basename $$file) ; \
		echo $$file ; \
    done
clean:
	rm -rf sysroot/
setup:
	mkdir -p sysroot/
