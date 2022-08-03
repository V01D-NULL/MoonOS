ARCH = 
LIMINE_DIR = meta/limine
ECHFS_DIR =  meta/echfs

CC = @gcc
AR = @ar
LD = @ld.lld
AS = @nasm

EMU = qemu-system-x86_64
EMU_OPTS = \
        -M q35 -cpu qemu64 -m 1G \
        -serial stdio -no-reboot -smp 4 \
        -no-shutdown -cdrom $(ISO_NAME)

EMU_OPTS_KVM = -enable-kvm -cpu host
EMU_OPTS_CUTTING_EDGE = -cpu qemu64,+la57 -bios /usr/share/ovmf/OVMF.fd
EMU_DEBUG_OPTS = $(EMU_OPTS) -S -s --no-reboot

CFILES	 := $(shell find ./ -type f -name '*.c')
ASMFILES := $(shell find ./ -type f -name '*.s')
OBJ_C    := $(CFILES:.c=.o)
OBJ_ASM  := $(ASMFILES:.s=.o)
OBJ		 := $(OBJ_C) $(OBJ_ASM)

ISO_NAME	  = MoonOS.iso 

# Kernel compiler / linker flags
CFLAGS := 				 \
	-I ../libs/			 \
	-I ../kernel/		 \
	-std=gnu11			 \
	-O2	-pipe -g		 \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-fpie				 \
	-lgcc				 \
	-mno-sse			 \
	-mno-sse2			 \
	-Wall				 \
	-fno-omit-frame-pointer \
	-mno-red-zone

ASMFLAGS = -felf64 -g -F dwarf
LDINTERNALFLAGS :=  \
	-Tlinker.ld     \
	-static         \
	-pie			\
	--no-dynamic-linker \
	-ztext			\
	-zmax-page-size=0x1000 \
	-nostdlib      
