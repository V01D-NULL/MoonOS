KERNEL_HDD 	   = ValidityOS.hdd
BUILD_UTIL_DIR = build-util

.PHONY: clean all run

all: quick_recompile  $(KERNEL_HDD)

run: $(KERNEL_HDD)
	qemu-system-x86_64 -m 2G -serial mon:stdio -hda $(KERNEL_HDD) -d cpu_reset

kernel/Validity.elf:
	$(MAKE) -C kernel

$(KERNEL_HDD): kernel/Validity.elf
	dd if=/dev/zero bs=1M count=0 seek=64 of=$(KERNEL_HDD)
	parted -s $(KERNEL_HDD) mklabel gpt
	parted -s $(KERNEL_HDD) mkpart primary 2048s 100%
	./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) quick-format 512
	./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import kernel/Validity.elf Validity.elf
	./$(BUILD_UTIL_DIR)/echfs-utils -g -p0 $(KERNEL_HDD) import boot/limine.cfg limine.cfg
	./$(BUILD_UTIL_DIR)/limine-install 	   $(KERNEL_HDD)

clean:
	rm -f $(KERNEL_HDD)
	$(MAKE) -C kernel clean


# Remove the HDD file and save all object files (fewer files will be recompiled)
quick_recompile:
	rm -f $(KERNEL_HDD)