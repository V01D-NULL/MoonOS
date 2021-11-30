CC = @gcc
TARGET := init.elf
CFILES	 := $(shell find ./ -type f -name '*.c')
OBJ    := $(CFILES:.c=.o)

.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 \
	-pipe -g -fno-pie -fno-pic -no-pie \
	-mno-sse -mno-sse2 -Wall		   \

all: $(TARGET)

$(TARGET): $(OBJ)
	ld.lld -T../daemon.ld $^ -static -nostdlib -o $@
	@mv $@ ../../boot/modules
	@printf "Built init daemon\n";

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
