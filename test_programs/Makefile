NAME = testprograms-$(OSNAME)

OUTPUT = ../disk/

.PHONY: clean setup iso

build:
	@export NAME=$(NAME) && cd test/ && $(MAKE) && $(MAKE) copy
	@export NAME=$(NAME) && cd shell/ && $(MAKE) && $(MAKE) copy
	@export NAME=$(NAME) && cd coreutils/ && $(MAKE) && $(MAKE) copy
iso:
	for file in sysroot/* ; do \
        cp $$file $(OUTPUT) ; \
		echo $$file ; \
    done
clean:
	rm -rf sysroot/
	@export NAME=$(NAME) && cd test/ && $(MAKE) clean
	@export NAME=$(NAME) && cd shell/ && $(MAKE) clean
	@export NAME=$(NAME) && cd coreutils/ && $(MAKE) clean
setup:
	mkdir -p sysroot/
