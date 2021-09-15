/**
 * @file serial.h
 * @author Tim (V01D)
 * @brief Reading and writing to the serial port
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef SERIAL_H
#define SERIAL_H

#include <libk/kprintf.h>
#include <libk/kstring.h>
#include <util/common.h>
#include <stdint.h>

#define COM1 0x3F8
#define COM2 0x2F8
#define COM3 0x3E8
#define COM4 0x2E8

#define BASH_RED     "\033[31m"
#define BASH_CYAN    "\033[36m"
#define BASH_GREEN   "\033[32m"
#define BASH_WHITE   "\033[97m"
#define BASH_YELLOW  "\033[93m"
#define BASH_DEFAULT "\033[39m"

void serial_write(char chr);
char serial_read();
void serial_write_str(const char* str);
void serial_set_color(const char* color_code);

#define debug(log_verbose, ...) if (log_verbose) impl_verbose_debug(__FILE__, __LINE__, __VA_ARGS__); else impl_quiet_debug(__VA_ARGS__);

/// Output a formatted string to the serial port
void impl_verbose_debug(char *file, int line, char* fmt, ...);
void impl_quiet_debug(char *fmt, ...);

#endif