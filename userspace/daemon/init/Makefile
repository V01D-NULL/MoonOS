CC = @gcc
TARGET := init.elf
CFILES	 := $(shell find ./ -type f -name '*.c')
CFILE_DEPS := $(shell find ../../abi -type f -name '*.c') # Grab abi/ files
OBJ    := $(CFILE_DEPS:.c=.o) $(CFILES:.c=.o)

.PHONY: all clean

CFLAGS = -ffreestanding -O2 -std=gnu11 \
	-mno-sse -mno-sse2 -Wall -I../../ \

all: $(TARGET)
	@printf "Built init daemon\n";

$(TARGET): $(OBJ)
	@ld.lld -T../daemon.ld $^ -static -nostdlib -o $@
	@mv $@ ../../../boot/modules
	
%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@
#	nasm -f elf64 entry.s -o test.o

clean:
	@rm -rf $(TARGET) $(OBJ)
