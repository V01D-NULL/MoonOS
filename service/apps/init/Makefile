-include ../../../Make.conf

CC = @clang
TARGET := init.elf
CFILES	 := $(shell find ./ -type f -name '*.c')
USER_LIBS := $(shell find ../../../userspace -type f -name '*.a')
OBJ    := $(CFILES:.c=.o)
.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 \
	-mno-sse -mno-sse2 -Wall -I../../../userspace/arch/$(ARCH)/ \

all: $(TARGET)
	@printf "Built init service\n\n";

$(TARGET): $(OBJ)
	@ld.lld -T../service.ld $^ ../../libc/libc.a $(USER_LIBS) -static -nostdlib -o $@
	@mv $@ ../../../boot/modules
	
%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
