include ../build-util/flags.mk

TARGET := kernel.elf

.PHONY: all clean

all: $(TARGET)
	@printf "\n\[^*^]/ Kernel compiled & linked successfully \[^*^]/ \n\n";

$(TARGET): $(OBJ)
	@printf " LD  Linking object files\n";
	$(LD) $(LDINTERNALFLAGS) ./drivers/gfx/__font__.o $(OBJ) $(KLIBS)/libk.a $(KLIBS)/liballoc.a -o $@

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf " AS  $<\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
