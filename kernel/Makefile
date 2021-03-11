TARGET := Validity.elf
SRCDIR  = src/

CC = @cc
AS = @nasm
LD = @ld
CFLAGS := 				 \
	-O2	-pipe -g		 \
	-ffreestanding       \
	-fno-stack-protector \
	-fno-pic             \
	-mno-80387           \
	-mno-mmx             \
	-mno-3dnow           \
	-mno-sse             \
	-mno-sse2            \
	-mcmodel=kernel      \
	-mno-red-zone

ASMFLAGS = -felf64 -g -F dwarf
LDINTERNALFLAGS := \
	-Tlinker.ld    \
	-static        \
	-nostdlib      \
	-no-pie		   

CFILES	 := $(shell find ./ -type f -name '*.c')
ASMFILES := $(shell find ./ -type f -name '*.s')
OBJ_C    := $(CFILES:.c=.o)
OBJ_ASM  := $(ASMFILES:.s=.o)
OBJ		 := $(OBJ_C) $(OBJ_ASM)

.PHONY: all clean

all: $(TARGET)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

$(TARGET): $(OBJ)
	@printf "[LD] Linking object files\n";
	$(LD) $(LDINTERNALFLAGS) $(OBJ) -o $@

$(SRCDIR)%.o: $(SRCDIR)%.c
	@printf "[CC] [ $< ]\n";
	$(CC) $(CFLAGS) -c $< -o $@

$(SRCDIR)%.o: $(SRCDIR)%.s
	@printf "[AS] [ $< ]\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	rm -rf $(TARGET) $(OBJ)
