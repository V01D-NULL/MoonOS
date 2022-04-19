#ifndef LIB_GRAPHICS_DOUBLE_BUFFERING_H
#define LIB_GRAPHICS_DOUBLE_BUFFERING_H

#include <stdint.h>
#include <stdbool.h>
#include <boot/boot.h>

typedef struct gfx_header
{
    uint64_t fb_addr;
    uint16_t fb_width;
    uint16_t fb_height;
    uint16_t fb_pitch;
    uint16_t fb_bpp;
    uint32_t last_known_fg;
    uint32_t last_known_bg;
} gfx_header_t;

void double_buffering_init(BootContext);
void swap_buffers(uint32_t *custom_buffer);
void buffer_pixel(int x, int y, int color);
void flush_back_buffer(uint32_t *custom_buffer);
uint32_t *double_buffering_create_buffer(void);

#endif // LIB_GRAPHICS_DOUBLE_BUFFERING_H