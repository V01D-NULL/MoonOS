include ../../Make.conf
include ../../meta/build/make/Makefile.$(ARCH).$(BOARD).conf

TARGET := base.a

CFILES	 := $(shell find ./ -type f -name '*.c')
ASMFILES := $(shell find ./ -type f -name '*.s')
OBJ		 := $(ASMFILES:.s=.o) $(CFILES:.c=.o)

.PHONY: all clean

CFLAGS += -I ../../kernel/ \
		  -I ../ -I . \
		  -I ../../kernel/arch/$(ARCH)/

all: $(TARGET)

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
