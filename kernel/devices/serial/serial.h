#ifndef SERIAL_H
#define SERIAL_H

#include <libk/kprintf.h>
#include <libk/kstring.h>
#include <util/common.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define BASH_RED     "\033[31m"
#define BASH_CYAN    "\033[36m"
#define BASH_GREEN   "\033[32m"
#define BASH_WHITE   "\033[97m"
#define BASH_GRAY    "\033[37m"
#define BASH_PURPLE  "\033[35m"
#define BASH_YELLOW  "\033[93m"
#define BASH_DEFAULT "\033[39m"

void serial_write(char chr);
char serial_read(void);
void serial_write_str(string_view str);
void serial_set_color(string_view color_code);

#define debug(log_verbose, ...) if (log_verbose) impl_verbose_debug(__FILE__, __LINE__, __VA_ARGS__); else impl_quiet_debug(__VA_ARGS__);

/// Output a formatted string to the serial port
void impl_verbose_debug(string file, int line, string  fmt, ...);
void impl_quiet_debug(string fmt, ...);

#endif