include ../build-util/flags.mk

TARGET := kernel.elf
KLIBS  := $(shell find ../libs -type f -name '*.a')

.PHONY: all clean

all: $(TARGET)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

$(TARGET): $(OBJ)
	@printf " LD  Linking object files\n";
	$(LD) $(LDINTERNALFLAGS) ./drivers/vbe/__font__.o $(OBJ) $(KLIBS) ./trace/symtable.o -o $@
	python3 ../scripts/gensym.py $@
	$(LD) $(LDINTERNALFLAGS) ./drivers/vbe/__font__.o $(OBJ) $(KLIBS) ./trace/symtable.o -o $@

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf " AS  $<\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
