ARCH = 

CC = @$(ARCH)-gcc
AR = @$(ARCH)-ar
LD = @$(ARCH)-ld
AS = @nasm

EMU = qemu-system-x86_64
EMU_OPTS = \
        -M q35 -cpu host -m 256M \
        -enable-kvm -serial stdio -no-reboot \
        -no-shutdown -rtc base=localtime -cdrom $(ISO_NAME)
EMU_OPTS_KVM = -M q35 -cpu host -m 256M -enable-kvm -serial stdio -no-reboot -no-shutdown -cdrom $(ISO_NAME) -d guest_errors
EMU_DEBUG_OPTS = $(EMU_OPTS) -S -s --no-reboot

CFILES	 := $(shell find ./ -type f -name '*.c')
ASMFILES := $(shell find ./ -type f -name '*.s')
OBJ_C    := $(CFILES:.c=.o)
OBJ_ASM  := $(ASMFILES:.s=.o)
OBJ		 := $(OBJ_C) $(OBJ_ASM)

ISO_NAME	  = ValidityOS.iso 

KLIBS 	 := ../libs
FONT 	 := font/console.sfn

# Kernel compiler / linker flags
CFLAGS := 				 \
	-I ../libs/			 \
	-std=c11			 \
	-O2	-pipe -g		 \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mcmodel=kernel      \
	-fno-pie			 \
	-fno-pic			 \
	-no-pie				 \
	-lgcc				 \
	-mno-sse			 \
	-mno-sse2			 \
	-pedantic			 \
	-Wall				 \
	-Werror				 \
	-mno-red-zone

ASMFLAGS = -felf64 -g -F dwarf
LDINTERNALFLAGS :=  \
	-Tlinker.ld     \
	-static         \
	-nostdlib      
