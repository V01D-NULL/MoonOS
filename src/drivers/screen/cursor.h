#ifndef VGA_CURSOR_H
#define VGA_CURSOR_H

#include "../../common.h"
#include "monitor.h"

void move_cursor();
void kill_cursor();
void enable_cursor(u8int cursor_start, u8int cursor_end);

#endif