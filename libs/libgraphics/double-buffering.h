#ifndef LIB_GRAPHICS_DOUBLE_BUFFERING_H
#define LIB_GRAPHICS_DOUBLE_BUFFERING_H

#include <stdint.h>
#include <stdbool.h>

void double_buffering_init();
void swap_buffers();
void buffer_pixel(int x, int y, int color);
void flush_back_buffer();

#endif // LIB_GRAPHICS_DOUBLE_BUFFERING_H