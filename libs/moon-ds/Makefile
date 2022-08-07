include ../../Make.conf
include ../../meta/build/make/Makefile.$(ARCH).$(BOARD).conf

TARGET := moon-ds.a

CFILES	 := $(shell find ./ -type f -name '*.c')
ASMFILES := $(shell find ./ -type f -name '*.s')
OBJ_C    := $(CFILES:.c=.o)
OBJ_ASM  := $(ASMFILES:.s=.o)
OBJ		 := $(OBJ_C) $(OBJ_ASM)

.PHONY: all clean

CFLAGS += -I ../../kernel/ \
		  -I ../

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
