include ../Make.conf

TARGET := kernel.elf
KLIBS  := $(shell find ../libs -type f -name '*.a')

CFILES	 := $(shell find ./arch/$(ARCH) -type f -name '*.c') \
			$(shell find $(PLATFORM_DIR) -name '*.c' -not -path './arch/*') \
			$(shell find ./ -name '*.c' -not -path './arch/*' -and -not -path './platform/*')

ASMFILES := $(shell find ./arch/$(ARCH) -type f -name '*.s') \
			$(shell find $(PLATFORM_DIR) -name '*.s' -not -path './arch/*') \
			$(shell find ./ -name '*.s' -not -path './arch/*' -not -path './platform/*')

OBJ := $(ASMFILES:.s=.o) $(CFILES:.c=.o)

.PHONY: all clean

all: $(TARGET)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf " AS  $<\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)

# Rules from this makefile are called after compiling the kernel.
# They contain linker and post build commands (if any).
include ../meta/build/make/Makefile.$(ARCH).$(BOARD).conf
