KERNEL_HDD 	   = MoonOS.hdd
KERNEL_ELF	   = kernel/kernel.elf

# CHANGE ME! I will be the terminal in which you can debug MoonOS.
DEBUG_TERMINAL = gnome-terminal
DEBUG_TERMINAL_OPTS = --tab "[DEBUG] MoonOS [DEBUG]" --command

-include Make.conf

.PHONY: clean all run

all: symlist quick_recompile $(KERNEL_HDD) ISO

daemons:
	@$(MAKE) --no-print-directory -C userspace/daemon/init

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
	@$(MAKE) --no-print-directory -C $(ECHFS_DIR)
	@$(MAKE) --no-print-directory -C $(LIMINE_DIR)
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD) 								2> /dev/null
	@parted -s $(KERNEL_HDD) mklabel gpt 													2> /dev/null
	@parted -s $(KERNEL_HDD) mkpart primary 2048s 100% 										2> /dev/null
	@$(ECHFS_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) quick-format 512 								2> /dev/null
	@$(ECHFS_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import $^ kernel.elf							2> /dev/null
	@$(ECHFS_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import boot/limine.cfg limine.cfg				2> /dev/null
	@$(LIMINE_DIR)/limine-install 	$(KERNEL_HDD)													2> /dev/null

symlist:
	@echo '#include "sym.h"' > scripts/parsed.sym
	@echo 'SymbolTable symbol_table[] = {{0xFFFFFFFFFFFFFFFF, ""}};' >> scripts/parsed.sym
	$(CC) -x c $(CHARDFLAGS) -I kernel/trace -I libs/ -m64 -c scripts/parsed.sym -o kernel/trace/symtable.o

libs:
	@$(MAKE) --no-print-directory -C libs all

clean:
	@rm -f $(KERNEL_HDD)
	@$(MAKE) --no-print-directory -C kernel clean
	@$(MAKE) --no-print-directory -C libs   clean
	@$(MAKE) --no-print-directory -C daemon/init   clean
	@$(MAKE) --no-print-directory -C userspace clean

debugger_session: $(KERNEL_HDD)
	$(DEBUG_TERMINAL) $(DEBUG_TERMINAL_OPTS) ./debug-util/debug.sh &
	$(EMU) $(EMU_DEBUG_OPTS)

ISO: $(KERNEL_HDD)
	mkdir iso/ || echo ""
	@cp -r $(LIMINE_DIR)/BOOTIA32.EFI $(LIMINE_DIR)/BOOTX64.EFI $(LIMINE_DIR)/limine.sys $(LIMINE_DIR)/limine-cd.bin \
	$(LIMINE_DIR)/limine-eltorito-efi.bin $(LIMINE_DIR)/limine-pxe.bin boot/* $(KERNEL_ELF) iso/

	xorriso -as mkisofs -b limine-cd.bin \
	-no-emul-boot -boot-load-size 4 -boot-info-table \
	--efi-boot limine-eltorito-efi.bin -efi-boot-part \
	--efi-boot-image --protective-msdos-label iso -o $(ISO_NAME)

	$(LIMINE_DIR)/limine-install $(ISO_NAME)

# Remove the HDD & elf file while saving all object files (fewer files will be recompiled)
quick_recompile: symlist
	@rm -f $(KERNEL_HDD) kernel/kernel.elf
	@printf "\n"
	@$(MAKE) --no-print-directory -C libs
	@$(MAKE) --no-print-directory -C userspace/
	@$(MAKE) --no-print-directory -C daemon/init
	@$(MAKE) --no-print-directory -C kernel
