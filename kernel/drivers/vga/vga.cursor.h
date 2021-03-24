#ifndef VGA_CURSOR_H
#define VGA_CURSOR_H

#include <stdint.h>

void vga_move_cursor();
void vga_kill_cursor();
void vga_enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

#endif