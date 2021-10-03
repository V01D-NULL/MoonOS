#include "double-buffering.h"
#include <drivers/vbe/vbe.h>
#include <printk.h>
#include <mm/heap/heap.h>
 
static struct display front;
static struct display  back;
extern gfx_header_t gfx_h;
struct display get_active();

void double_buffering_init()
{
    front.is_front = false;
    back.is_front  = true;

    front.buffer = (uint32_t*) heap_alloc(gfx_h.fb_width * gfx_h.fb_height * (gfx_h.fb_bpp / 8)).base;
    back.buffer  = (uint32_t*) heap_alloc(gfx_h.fb_width * gfx_h.fb_height * (gfx_h.fb_bpp / 8)).base;
}

static inline void render_buffer(struct display renderer)
{
    /* Render the contents of the backbuffer onto the display */
    memcpy32((uint32_t*)gfx_h.fb_addr, renderer.buffer, gfx_h.fb_height * gfx_h.fb_pitch);
    // memset(renderer.buffer, 0, gfx_h.fb_height * gfx_h.fb_pitch);
}

void swap_buffers()
{
    back.is_front ? render_buffer(front) : render_buffer(back);
    front.is_front = !front.is_front;
    back.is_front = !back.is_front;
}

void buffer_pixel(int x, int y, int color)
{
    get_active().buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
}

void fill_rect(int x1, int y1, int x2, int y2, int color)
{
    for (int y = y1; y < y2; y++)
    {
        for (int x = x1; x < x2; x++)
        {
            get_active().buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = 0xFFFFFF;
        }
    }

    swap_buffers();
}

/* Return the current BACK buffer */
struct display get_active()
{
    if (front.is_front)
        return back;
 
    return front;
}