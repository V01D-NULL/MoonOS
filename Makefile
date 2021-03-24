KERNEL_HDD 	   = ValidityOS.hdd
KERNEL_ELF	   = kernel/Validity.elf
BUILD_UTIL_DIR = build-util

# CHANGE ME! I will be the terminal in which you can debug ValidityOS.
DEBUG_TERMINAL = mate-terminal
DEBUG_TERMINAL_OPTS = -t "[DEBUG] ValidityOS [DEBUG]" --command

include build-util/flags.mk

.PHONY: clean all run

all: klibs quick_recompile $(KERNEL_HDD)

run: klibs quick_recompile $(KERNEL_HDD)
	$(EMU) $(EMU_OPTS)

$(KERNEL_ELF):
	$(MAKE) --no-print-directory -C kernel

$(KERNEL_HDD): $(KERNEL_ELF)
	@dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD)
	@parted -s $(KERNEL_HDD) mklabel gpt
	@parted -s $(KERNEL_HDD) mkpart primary 2048s 100%
	@./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) quick-format 512
	@./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import $^ Validity.elf
	@./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import boot/limine.cfg limine.cfg
	@./$(BUILD_UTIL_DIR)/limine-install 	$(KERNEL_HDD)

klibs:
	$(MAKE) --no-print-directory -C libs all

clean:
	@rm -f $(KERNEL_HDD)
	$(MAKE) --no-print-directory -C kernel clean
	$(MAKE) --no-print-directory -C libs   clean

debugger_session: $(KERNEL_HDD)
	$(DEBUG_TERMINAL) $(DEBUG_TERMINAL_OPTS) ./debug-util/debug.sh &
	$(EMU) $(EMU_DEBUG_OPTS)


# Remove the HDD & elf file while saving all object files (fewer files will be recompiled)
quick_recompile:
	@rm -f $(KERNEL_HDD) kernel/Validity.elf
	@printf "\n";
	$(MAKE) --no-print-directory -C kernel
