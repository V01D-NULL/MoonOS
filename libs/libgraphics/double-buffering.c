#include <drivers/vbe/vbe.h>
#include <mm/heap/heap.h>

static uint32_t *buffer;
extern gfx_header_t gfx_h;

void double_buffering_init()
{
    buffer  = (uint32_t*) heap_alloc(gfx_h.fb_width * gfx_h.fb_height * (gfx_h.fb_bpp / 8)).base;
}

void swap_buffers()
{
    memcpy32((uint32_t*)gfx_h.fb_addr, buffer, gfx_h.fb_height * gfx_h.fb_pitch);
}

void buffer_pixel(int x, int y, int color)
{
    buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
}

void fill_rect(int x1, int y1, int x2, int y2, int color)
{
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
        }
    }

    swap_buffers();
}