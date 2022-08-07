-include ../../Make.conf

CC = @clang
TARGET := init.elf
CFILES	 := $(shell find ./ -type f -name '*.c')
USER_LIBS := $(shell find ../../userspace -type f -name '*.a')
OBJ    := $(CFILES:.c=.o)
.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 \
	-mno-sse -mno-sse2 -Wall -I../../userspace/arch/$(ARCH)/ \

all: $(TARGET)
	@printf "Built init daemon\n\n";

$(TARGET): $(OBJ)
	@ld.lld -T../daemon.ld $^ $(USER_LIBS) -static -nostdlib -o $@
	@mv $@ ../../boot/modules
	
%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@
#	nasm -f elf64 entry.s -o test.o

clean:
	@rm -rf $(TARGET) $(OBJ)
