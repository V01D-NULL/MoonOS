KERNEL_HDD 	   = MoonOS.hdd
KERNEL_ELF	   = kernel/kernel.elf

# CHANGE ME! I will be the terminal in which you can debug MoonOS.
DEBUG_TERMINAL = gnome-terminal
DEBUG_TERMINAL_OPTS = --tab "[DEBUG] MoonOS [DEBUG]" --command

include ./flags.mk

.PHONY: clean all run

all: daemons symlist quick_recompile $(KERNEL_HDD) ISO

daemons:
	@$(MAKE) --no-print-directory -C daemon/init

run: quick_recompile ISO
ifeq ($(modern), yes)
	$(EMU) $(EMU_OPTS) $(EMU_OPTS_CUTTING_EDGE)
else
	$(EMU) $(EMU_OPTS)
endif

# modern=yes will attempt to use modern features. If the cpu does not support it, it will fallback to something else
kvm: quick_recompile ISO
ifeq ($(modern), yes)
	$(EMU) $(EMU_OPTS) $(EMU_OPTS_KVM) $(EMU_OPTS_CUTTING_EDGE)
else
	$(EMU) $(EMU_OPTS) $(EMU_OPTS_KVM)
endif

$(KERNEL_ELF):
	@$(MAKE) --no-print-directory -C kernel

$(KERNEL_HDD): $(KERNEL_ELF)
	@$(MAKE) --no-print-directory -C echfs
	@$(MAKE) --no-print-directory -C limine
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD) 								2> /dev/null
	@parted -s $(KERNEL_HDD) mklabel gpt 													2> /dev/null
	@parted -s $(KERNEL_HDD) mkpart primary 2048s 100% 										2> /dev/null
	@echfs/echfs-utils -g -p0 $(KERNEL_HDD) quick-format 512 								2> /dev/null
	@echfs/echfs-utils -g -p0 $(KERNEL_HDD) import $^ kernel.elf							2> /dev/null
	@echfs/echfs-utils -g -p0 $(KERNEL_HDD) import boot/limine.cfg limine.cfg				2> /dev/null
	@limine/limine-install 	$(KERNEL_HDD)													2> /dev/null
	
symlist:
	@echo '#include "sym.h"' > scripts/parsed.sym
	@echo 'sym_table_t symbol_table[] = {{0xFFFFFFFFFFFFFFFF, ""}};' >> scripts/parsed.sym
	$(CC) -x c $(CHARDFLAGS) -I kernel/trace -m64 -c scripts/parsed.sym -o kernel/trace/symtable.o

klibs:
	@$(MAKE) --no-print-directory -C libs all

clean:
	@rm -f $(KERNEL_HDD)
	@$(MAKE) --no-print-directory -C kernel clean
	@$(MAKE) --no-print-directory -C libs   clean
	@$(MAKE) --no-print-directory -C daemon/init   clean

debugger_session: $(KERNEL_HDD)
	$(DEBUG_TERMINAL) $(DEBUG_TERMINAL_OPTS) ./debug-util/debug.sh &
	$(EMU) $(EMU_DEBUG_OPTS)

ISO: $(KERNEL_HDD)
	mkdir iso/ || echo ""
	@cp -r limine/BOOTIA32.EFI limine/BOOTX64.EFI limine/limine.sys limine/limine-cd.bin \
	limine/limine-eltorito-efi.bin limine/limine-pxe.bin boot/* $(KERNEL_ELF) iso/

	xorriso -as mkisofs -b limine-cd.bin \
	-no-emul-boot -boot-load-size 4 -boot-info-table \
	--efi-boot limine-eltorito-efi.bin -efi-boot-part \
	--efi-boot-image --protective-msdos-label iso -o $(ISO_NAME)

	limine/limine-install $(ISO_NAME)

# Remove the HDD & elf file while saving all object files (fewer files will be recompiled)
quick_recompile: symlist
	@rm -f $(KERNEL_HDD) kernel/kernel.elf
	@printf "\n";
	@$(MAKE) --no-print-directory -C libs
	@printf "\n"
	@$(MAKE) --no-print-directory -C kernel
	@printf "\n"
	@$(MAKE) --no-print-directory -C daemon/init
