#ifndef LEGACY_VGA_H
#define LEGACY_VGA_H

#include "../../libk/kstring.h"
#include "../../libk/kstdlib.h"
#include "../../libk/kprintf.h"
#include "../io/serial.h"
#include <stdarg.h>
#include "vga.cursor.h"
#include <stdint.h>
#include <stddef.h>

typedef struct vga_hdr {
    uint8_t  col;
    uint8_t  row;
    uint8_t  cursor_x;
    uint8_t  cursor_y;
    uint16_t *vram;
    uint16_t attr; //Cell attribute (fg/bg color)
    int      text_scroll_speed;
} vga_hdr_t;

typedef struct vga_history {
    int cell_color_fg;
    int cell_color_bg;
} vga_history_t;

//Init structs
vga_hdr_t vga;
vga_history_t vga_hist;

//
void vga_init(int fg, int bg);

// Scrolls the text on the screen down by one line
void vga_scroll();

// Write a single character out to the screen
void vga_putc(char c);

// Clear the screen to all black
void vga_clear();

// Output a null-terminated ASCII string to the monitor
void vga_puts(char *c, bool is_task, bool operation_ok);

//
int debug(char* fmt, ...);

//
int kprintf(const char* fmt, ...);

// Kernel Printf Pretty (utilises the two boolean paramters that vga_puts has to offer. For kprintf these are both false)
int kprintf_p(bool is_task, bool operation_ok, const char* fmt,  ...);

// Set the cell's color attributes
void set_color(const int BGCOLOR, const int FGCOLOR);

// Set the horizontal scrolling speed
void set_scroll_speed(uint32_t speed);

// " [ OK ] " (with color coding)
void op_ok();

// "[TASK]" (with color coding)
void op_task();


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

#endif  // LEGACY_VGA_H
