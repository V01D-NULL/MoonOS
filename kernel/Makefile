include ../flags.mk

TARGET := kernel.elf
KLIBS  := $(shell find ../libs -type f -name '*.a')

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
