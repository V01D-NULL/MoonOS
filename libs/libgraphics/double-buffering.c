#include "double-buffering.h"
#include <libk/kstring.h>
#include <mm/heap/heap.h>

static uint32_t *buffer;
static gfx_header_t gfx_h;

void double_buffering_init(boot_info_t *boot)
{
    gfx_h.fb_addr = boot->vesa.fb_addr;
    gfx_h.fb_bpp  = boot->vesa.fb_bpp;
    gfx_h.fb_height = boot->vesa.fb_height;
    gfx_h.fb_width = boot->vesa.fb_width;
    gfx_h.fb_pitch = boot->vesa.fb_pitch;

    buffer  = (uint32_t*) heap_alloc(gfx_h.fb_width * gfx_h.fb_height * (gfx_h.fb_bpp / 8)).base;
    flush_back_buffer();
}

void swap_buffers()
{
    memcpy32((uint32_t*)gfx_h.fb_addr, buffer, gfx_h.fb_height * gfx_h.fb_pitch);
}

void buffer_pixel(int x, int y, int color)
{
    buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
}

void flush_back_buffer()
{
    memset(buffer, 0, gfx_h.fb_height * gfx_h.fb_pitch);
    swap_buffers();
}