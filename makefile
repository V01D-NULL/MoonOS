#
# Makefile
#
SHELL=/bin/sh

CFILES    := $(shell find ./src -type f -name '*.c')
ASMFILES  := $(shell find ./src -type f -name '*.s')
OBJ_C     := $(CFILES:.c=.o)
OBJ_ASM	  := $(ASMFILES:.s=.o)
OBJ		   = $(OBJ_C) $(OBJ_ASM)

CC = @gcc
AS = @nasm
LD = @ld

# Images
ELF    	 = kernel.elf
INITRD 	 = initrd.img

# 32 bit flags
CFLAGS  = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -m32 -Wall -g
ASFLAGS = -f elf32 -F dwarf -g
LDFLAGS = -T link.ld -m elf_i386

# 64 bit flags
# CFLAGS  = -nostdlib -nostdinc -fno-builtin -fno-stack-protector -Wall -g
# ASFLAGS = -f elf64 -F dwarf -g
# LDFLAGS = -T link.ld


# Generate ELF ----------------------

all:
	make _all
	
get_compiler:
	@printf "\n\n[_____ Using the $(CC) and $(AS) compiler. _______]\n\n";

_all: get_compiler $(OBJ_ASM) $(OBJ_C) link

clean:
	@printf "[CLEAN] Cleaning object files\n";
	@rm $(OBJ)

link:
	@printf "[LD] Linking object files\n";
	$(LD) $(LDFLAGS) -o $(ELF) $(OBJ_ASM) $(OBJ_C)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

.s.o:
	@printf "[AS] $<\n";
	$(AS) $(ASFLAGS) $<

.c.o:
	@printf "[CC] $<\n";
	$(CC) -c $(CFLAGS) $< -o $@

# Build iso for bootable USB drive / Build a GRUB iso
ISO				= ValidityOS.iso
ISO_DIR 		= iso/boot/grub/themes/Validity
ELF_DIR 		= iso/boot/$(ELF)
GRUB_CFG_ORIG	= validity_grub/grub.cfg
GRUB_CFG_DIR	= iso/boot/grub/grub.cfg
GRUB_THEME_TXT	= iso/boot/grub/themes/Validity/theme.txt
GRUB_THEME_ORIG = validity_grub/theme.txt
grub:
	@printf "[GRUB] Preparing to creat a bootable ISO...\n";
	@printf "[GRUB] Copying files...\n";
	@mkdir -p $(ISO_DIR)
	@cp $(ELF) $(ELF_DIR)
	@cp $(GRUB_CFG_ORIG) $(GRUB_CFG_DIR)
	@cp $(GRUB_THEME_ORIG) $(GRUB_THEME_TXT)
	@printf "[GRUB] Creating bootable ISO...\n";
	@grub-mkrescue -o $(ISO) iso || exit
	@printf "[GRUB] \[^*^]/ Built the Validity ISO with GRUB support! \[^*^]/\n";

# Run QEMU --------------------------

# Path to QEMU
QEMU = qemu-system-i386

# Flags
# -serial mon:stdio is used so that ^C won't make qemu exit
QEMUOPTS = -kernel $(ELF) -serial mon:stdio -d cpu_reset -soundhw pcspk -initrd $(INITRD) # -full-screen
QEMUOPTS_GRUB = -cdrom $(ISO) -serial mon:stdio -d cpu_reset -soundhw pcspk -m 4G

# Try to generate a unique GDB port
GDBPORT = $(shell expr `id -u` % 5000 + 25000)

# QEMU's gdb stub command line changed in 0.11
QEMUGDB = \
	$(shell if $(QEMU) -help | grep -q '^-gdb';  \
	then echo "-gdb tcp::$(GDBPORT)";            \
	else echo "-s -p $(GDBPORT)"; fi)

# Prompt user to setup GDB appropriately
gdbinit:

	@echo "\nOpen gdb in another window and type the following:"
	@#echo "   set architecture i386"
	@echo "   file kernel"
	@echo "   target remote :"$(GDBPORT)
	@echo "Use 'break' to set breakpoints, and 'continue' to execute.\n"

qemu:
	make all
	@echo "Starting QEMU ..."
	@echo "If using curses, press Esc+2 then type 'quit' to exit"

	@# $(QEMU) $(QEMUOPTS)
	$(QEMU) $(QEMUOPTS)

qemu-nox:
	make all
	@echo "Starting QEMU ..."
	$(QEMU) -nographic -monitor stdio $(QEMUOPTS)

qemu-gdb:
	make all
	make gdbinit
	@echo "Starting QEMU ..."
	$(QEMU) $(QEMUOPTS) -S $(QEMUGDB)

qemu-curses:
	make all
	make gdbinit
	
	@echo "Starting QEMU ..."
	@echo "Press Esc+2 then type 'quit' to exit"

	$(QEMU) -display curses $(QEMUOPTS)

run:
	$(QEMU) $(QEMUOPTS)

run_grub:
	$(QEMU) $(QEMUOPTS_GRUB)
