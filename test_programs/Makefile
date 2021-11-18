NAME = testprograms-$(OSNAME)

OUTPUT = ../disk/

INIT = shell.o

.PHONY: clean setup iso

build:
	@export NAME=$(NAME) && cd test/ && $(MAKE) && $(MAKE) copy
	@export NAME=$(NAME) && cd shell/ && $(MAKE) && $(MAKE) copy
	@export NAME=$(NAME) && cd coreutils/ && $(MAKE) && $(MAKE) copy
iso:
	mkdir -p $(OUTPUT)/{bin,sbin}
	cp sysroot/$(INIT) $(OUTPUT)/sbin/init
	for file in sysroot/* ; do \
		if [ "$$(basename $$file)" != "$(INIT)" ]; then \
	        cp $$file $(OUTPUT)/bin/$$(basename $$file .o) ; \
			echo $$file ; \
		fi \
    done
clean:
	rm -rf sysroot/
	@export NAME=$(NAME) && cd test/ && $(MAKE) clean
	@export NAME=$(NAME) && cd shell/ && $(MAKE) clean
	@export NAME=$(NAME) && cd coreutils/ && $(MAKE) clean
setup:
	mkdir -p sysroot/
