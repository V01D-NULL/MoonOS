FILE_PATH = ../FileSystemFiles
FILES = $(shell find $(FILE_PATH) -mindepth 1 | sed 's|^$(FILE_PATH)||')

CC = gcc
CFLAGS = -Wall

all:
	@printf "Files found:\n$(FILES)\n";
	@printf "Compiling initial ram disk...\n";
	$(CC) $(CFLAGS) fs_gen.c -o fs_gen.elf
	./fs_gen.elf $(FILE_PATH) $(FILES)