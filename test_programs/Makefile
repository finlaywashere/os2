build:
	@export NAME=$(NAME) && cd tests/ && $(MAKE) && $(MAKE) iso
	cd bash/ && ./build.sh
clean:
	@export NAME=$(NAME) && cd tests/ && $(MAKE) clean
setup:
	@export NAME=$(NAME) && cd tests/ && $(MAKE) setup
