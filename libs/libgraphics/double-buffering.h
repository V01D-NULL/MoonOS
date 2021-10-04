#ifndef LIB_GRAPHICS_DOUBLE_BUFFERING_H
#define LIB_GRAPHICS_DOUBLE_BUFFERING_H

#include <stdint.h>
#include <stdbool.h>
#include <boot/bootloader_stivale2.h>

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

void double_buffering_init(boot_info_t *);
void swap_buffers();
void buffer_pixel(int x, int y, int color);
void flush_back_buffer();

#endif // LIB_GRAPHICS_DOUBLE_BUFFERING_H