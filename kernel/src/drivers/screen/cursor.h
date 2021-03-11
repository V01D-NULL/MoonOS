#ifndef VGA_CURSOR_H
#define VGA_CURSOR_H

#include <stdint.h>
#include "monitor.h"

void move_cursor();
void kill_cursor();
void enable_cursor(uint8_t cursor_start, uint8_t cursor_end);

#endif