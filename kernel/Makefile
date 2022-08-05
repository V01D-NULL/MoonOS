include ../flags.mk

TARGET := kernel.elf
KLIBS  := $(shell find ../libs -type f -name '*.a')

ifeq ($(BOARD), nintendo)
	PLATFORM_DIR = ./platform/$(BOARD)
else
	PLATFORM_DIR = ./platform/$(BOARD)/$(ARCH)
endif

CFILES	 := $(shell find ./arch/$(ARCH) -type f -name '*.c') \
			$(shell find $(PLATFORM_DIR) -name '*.c' -not -path './arch/*') \
			$(shell find ./ -name '*.c' -not -path './arch/*' -and -not -path './platform/*')

ASMFILES := $(shell find ./arch/$(ARCH) -type f -name '*.s') \
			$(shell find $(PLATFORM_DIR) -name '*.s' -not -path './arch/*') \
			$(shell find ./ -name '*.s' -not -path './arch/*' -not -path './platform/*')

OBJ_C    := $(CFILES:.c=.o)
OBJ_ASM  := $(ASMFILES:.s=.o)
OBJ		 := $(OBJ_C) $(OBJ_ASM)

CFLAGS += -I arch/$(ARCH)/ -I $(PLATFORM_DIR)
ASMFLAGS += -I arch/$(ARCH)/

.PHONY: all clean

all: $(TARGET)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

$(TARGET): $(OBJ)
	@printf " LD  Linking object files\n";
	@ld -r -b binary -o font.o ../font.bin
	$(LD) $(LDINTERNALFLAGS) $(OBJ) $(KLIBS) ./trace/symtable.o font.o -o $@
	python3 ../scripts/gensym.py $@
	$(LD) $(LDINTERNALFLAGS) $(OBJ) $(KLIBS) ./trace/symtable.o font.o -o $@

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf " AS  $<\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
