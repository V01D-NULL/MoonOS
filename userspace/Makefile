include ../Make.conf

TARGET := userspace-components.a

CFILES	 := $(shell find ./arch/$(ARCH) -type f -name '*.c')
ASMFILES := $(shell find ./arch/$(ARCH) -type f -name '*.s')

OBJ_C    := $(CFILES:.c=.o)
OBJ_ASM  := $(ASMFILES:.s=.o)
OBJ		 := $(OBJ_C) $(OBJ_ASM)

.PHONY: all clean

CFLAGS := $(CFLAGS) \
			-I arch/$(ARCH)/abi

all: $(TARGET)
	@printf "(LIBS) Built all userspace archive libraries\n\n";

$(TARGET): $(OBJ)
	@printf " AR  Archiving object files\n";
	$(AR) rcs $@ $^

%.o: %.c
	@printf " CC  $<\n";
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@printf " AS  $<\n";
	$(AS) $(ASMFLAGS) $< -o $@

clean:
	@rm -rf $(TARGET) $(OBJ)
