-include ../../Make.conf

# TODO: Add support for other architectures
AS_FLAGS := -f elf64

TARGET := libc.a
CFILES := $(shell find ./ -type f -name '*.c') $(shell find ./$(ARCH) -type f -name '*.c')
SFILES := $(shell find ./ -type f -name '*.s') $(shell find ./$(ARCH) -type f -name '*.s')
OBJ    := $(CFILES:.c=.o) $(SFILES:.s=.o)
.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 \
	-mno-sse -mno-sse2 -Wall -I../../../userspace/arch/$(ARCH)/ \

all: $(TARGET)
	@printf "Built libc\n\n";

$(TARGET): $(OBJ)
	$(AR) rcs $@ $^

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf " AS  $<\n";
	$(AS) $(AS_FLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
