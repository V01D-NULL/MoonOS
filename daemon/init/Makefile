CC = @gcc
TARGET := init.elf
CFILES	 := $(shell find ./ -type f -name '*.c')
OBJ    := $(CFILES:.c=.o)

.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 \
	-pipe -fno-pie -fno-pic -no-pie \
	-mno-sse -mno-sse2 -Wall -I../../libs/ \

all: $(TARGET)
	@printf "Built init daemon\n";

$(TARGET): $(OBJ)
	@ld.lld -T../daemon.ld $^ -static -nostdlib -o $@
	@mv $@ ../../boot/modules
	
%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
