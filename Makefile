-include Make.conf

.PHONY: clean all run

all: symlist quick_recompile BOOT_IMAGE

run: quick_recompile BOOT_IMAGE
ifeq ($(modern), yes)
	$(EMU) $(EMU_OPTS) $(EMU_OPTS_CUTTING_EDGE)
else
	$(EMU) $(EMU_OPTS)
endif

# modern=yes will attempt to use modern features. If the cpu does not support it, it will fallback to something else
kvm: quick_recompile BOOT_IMAGE
ifeq ($(modern), yes)
	$(EMU) $(EMU_OPTS) $(EMU_OPTS_KVM) $(EMU_OPTS_CUTTING_EDGE)
else
	$(EMU) $(EMU_OPTS) $(EMU_OPTS_KVM)
endif

$(KERNEL_ELF):
	@$(MAKE) --no-print-directory -C kernel

symlist:
	@echo '#include "sym.h"' > scripts/parsed.sym
	@echo 'SymbolTable symbol_table[] = {{0xFFFFFFFFFFFFFFFF, ""}};' >> scripts/parsed.sym
	$(CC) -x c $(CHARDFLAGS) -I kernel/trace -I libs/ -m64 -c scripts/parsed.sym -o kernel/trace/symtable.o

libs:
	@$(MAKE) --no-print-directory -C libs all

clean:
	@$(MAKE) --no-print-directory -C kernel clean
	@$(MAKE) --no-print-directory -C libs   clean
	@$(MAKE) --no-print-directory -C service   clean
	@$(MAKE) --no-print-directory -C userspace clean

debug: quick_recompile
	$(DEBUG_TERMINAL) $(DEBUG_TERMINAL_OPTS) ./meta/debug-util/debug.sh &
	$(EMU) $(EMU_DEBUG_OPTS)

test:
	cd meta/tests && CEEDLING_MAIN_PROJECT_FILE=project-$(ARCH)-$(BOARD).yml ceedling

clean_test:
	cd meta/tests && CEEDLING_MAIN_PROJECT_FILE=project-$(ARCH)-$(BOARD).yml ceedling clean

distclean_test:
	rm -rf meta/tests/build

# FIXME: Use .d files instead of this
# Remove the elf file while saving all object files (fewer files will be recompiled)
quick_recompile: symlist
	@rm -f kernel/kernel.elf
	@printf "\n"
	@$(MAKE) --no-print-directory -C libs
	@$(MAKE) --no-print-directory -C userspace
	@$(MAKE) --no-print-directory -C service
	@$(MAKE) --no-print-directory -C kernel

-include meta/build/make/Makefile.$(ARCH).$(BOARD).conf