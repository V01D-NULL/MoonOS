#ifndef MONITOR_H
#define MONITOR_H

#include "../../libk/kstring.h"
#include "../../libk/kstdlib.h"

#include "../../stdarg.h"
#include "../io/serial.h"
#include "cursor.h"
#include <stdint.h>

#define NCOLS   80
#define NROWS   25

//Get position of X and Y
uint8_t get_x ();
uint8_t get_y ();

// Scrolls the text on the screen up by one line
void monitor_scrollUp ();

// Scrolls the text on the screen down by one line
void monitor_scrollDown ();

// Write a single character out to the screen
void monitor_put ( char c );

// Clear the screen to all black
void monitor_clear ();

// Output a null-terminated ASCII string to the monitor
void monitor_write (char *c, bool is_task, bool operation_ok);

// Write a newline character
void monitor_writeln ();

//
void monitor_write_hex ( uint64_t n );

//
void monitor_write_dec ( uint64_t n );

//
int debug(char* fmt, ...);

//
void set_color(const int BGCOLOR, const int FGCOLOR);

//
void set_scroll_speed(uint32_t speed);

//
int kprintf(const char* fmt, ...);

//
void op_ok();

//
void op_task();

//
void write_x(const char* str);

//
void write_x_dec(uint64_t n);

//
void write_x_hex(uint64_t n);

//
int kprintf_x(const char* fmt, ...);

enum vga_color {
    VGA_BLACK,
    VGA_BLUE,
    VGA_GREEN,
    VGA_CYAN,
    VGA_RED,
    VGA_MAGENTA,
    VGA_BROWN,
    VGA_LIGHT_GREY,
    VGA_DARK_GREY,
    VGA_LIGHT_BLUE,
    VGA_LIGHT_GREEN,
    VGA_LIGHT_CYAN,
    VGA_LIGHT_RED,
    VGA_LIGHT_MAGENTA,
    VGA_LIGHT_YELLOW,
    VGA_WHITE,
};

#endif  // MONITOR_H
