KERNEL_HDD 	   = ValidityOS.hdd
KERNEL_ELF	   = kernel/kernel.elf
BUILD_UTIL_DIR = build-util

# CHANGE ME! I will be the terminal in which you can debug ValidityOS.
DEBUG_TERMINAL = gnome-terminal
DEBUG_TERMINAL_OPTS = --tab "[DEBUG] ValidityOS [DEBUG]" --command

include build-util/flags.mk

.PHONY: clean all run

all: font quick_recompile $(KERNEL_HDD)

run: quick_recompile ISO
	$(EMU) $(EMU_OPTS)

kvm: quick_recompile ISO
	$(EMU) $(EMU_OPTS_KVM)

$(KERNEL_ELF):
	@$(MAKE) --no-print-directory -C kernel

$(KERNEL_HDD): $(KERNEL_ELF)
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD) 								2> /dev/null
	@parted -s $(KERNEL_HDD) mklabel gpt 													2> /dev/null
	@parted -s $(KERNEL_HDD) mkpart primary 2048s 100% 										2> /dev/null
	@./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) quick-format 512 					2> /dev/null
	@./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import $^ kernel.elf				2> /dev/null
	@./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import boot/limine.cfg limine.cfg 2> /dev/null
	@./$(BUILD_UTIL_DIR)/limine-install 	$(KERNEL_HDD)									2> /dev/null
	
klibs:
	@$(MAKE) --no-print-directory -C libs all

clean:
	@rm -f $(KERNEL_HDD)
	@$(MAKE) --no-print-directory -C kernel clean
	@$(MAKE) --no-print-directory -C libs   clean

debugger_session: $(KERNEL_HDD)
	$(DEBUG_TERMINAL) $(DEBUG_TERMINAL_OPTS) ./debug-util/debug.sh &
	$(EMU) $(EMU_DEBUG_OPTS)

ISO: $(KERNEL_HDD)
	mkdir iso/ || echo ""
	@cp limine/limine-cd.bin boot/
	@cp limine/limine.sys boot/
	@cp $(KERNEL_ELF) boot/kernel.elf
	@cp boot/* iso/
	xorriso -as mkisofs -b limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table iso/ -o ValidityOS.iso

# Remove the HDD & elf file while saving all object files (fewer files will be recompiled)
quick_recompile:
	@printf " LD  Creating font binary\n";
	$(LD) -r -b binary -o kernel/drivers/gfx/__font__.o font/console.sfn
	@rm -f $(KERNEL_HDD) kernel/kernel.elf
	@printf "\n";
	@$(MAKE) --no-print-directory -C libs
	@printf "\n"
	@$(MAKE) --no-print-directory -C kernel
