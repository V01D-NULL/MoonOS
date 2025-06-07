all: build tar

build:
	@$(MAKE) --no-print-directory -C libc	
	@$(MAKE) --no-print-directory -C apps

tar: build
	@tar cvf ../boot/modules/ramdisk $(shell find apps/ -name '*.elf' ! -name 'init.elf')

clean:
	@$(MAKE) --no-print-directory -C libc clean
	@$(MAKE) --no-print-directory -C apps clean