include ../build-util/flags.mk

TARGET := Validity.elf

.PHONY: all clean

all: $(TARGET)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

$(TARGET): $(OBJ)
	@printf "[kernel @ LD] Linking object files\n";
	$(LD) $(LDINTERNALFLAGS) $(OBJ) $(KLIBS)/libk.a -o $@

%.o: %.c
	@printf "[kernel @ CC] [ $< ]\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf "[kernel @ AS] [ $< ]\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
