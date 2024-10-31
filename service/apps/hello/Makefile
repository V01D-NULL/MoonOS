-include ../../../Make.conf

CC = @clang
TARGET := hello.elf
CFILES	 := $(shell find ./ -type f -name '*.c')
OBJ    := $(CFILES:.c=.o)
.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 -mno-sse -mno-sse2 -Wall -I../../libc/public/

all: $(TARGET)
	@printf "Built hello service\n\n";

$(TARGET): $(OBJ)
	@ld.lld -T../service.ld $^ ../../libc/libc.a -static -nostdlib -o $@
	@mv $@ ../../../boot/modules
	
%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
