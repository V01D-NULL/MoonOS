#ifndef SERIAL_H
#define SERIAL_H


#include <libk/kprintf.h>
#include <libk/kstring.h>
#include "../../common.h"
#include "../vga/vga.h"
#include <stdint.h>

/*
  COM ports 1&3 = IRQ#4
  COM ports 2&4 = IRQ#3
*/

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define BASH_RED "\e[31m"
#define BASH_WHITE "\e[97m"
#define BASH_YELLOW "\e[93m"
#define BASH_DEFAULT "\e[39m"

void serial_write(char chr);
char serial_read();
void serial_write_str(const char* str);
void serial_write_dec(uint64_t n);
void serial_write_hex(uint64_t n);
void serial_set_color(const char* color_code);

#endif