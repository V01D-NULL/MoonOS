#ifndef LIB_GRAPHICS_DOUBLE_BUFFERING_H
#define LIB_GRAPHICS_DOUBLE_BUFFERING_H

#include <stdint.h>
#include <stdbool.h>

struct display
{
    uint32_t *buffer;
    bool is_front;
};

void double_buffering_init();
void swap_buffers();
void buffer_pixel(int x, int y, int color);
void fill_rect(int x1, int y1, int x2, int y2, int color);

#endif // LIB_GRAPHICS_DOUBLE_BUFFERING_H