#include "double-buffering.h"
#include <drivers/vbe/vbe.h>
#include <printk.h>
#include <mm/pmm.h>
 
static struct display front;
static struct display  back;
extern gfx_header_t gfx_h;

void double_buffering_init()
{
    front.is_front = true;
    back.is_front  = false;

    // Todo: A kernel heap is required in order ot work with > 4096 bytes (which is the amount of bytes pmm_alloc returns, therefore double buffering will not work past 4096 of the framebuffer memory (thus resulting in UB and displaying rubbish to the display when swapping the buffers))
    // front.buffer = (uint32_t*) kmalloc(fb_width * fb_height * fb_bpp);
    // back.buffer  = (uint32_t*) kmalloc(fb_width * fb_height * fb_bpp);
}
 
void render_buffer(struct display renderer)
{
    memcpy32((uint32_t*)gfx_h.fb_addr, renderer.buffer, gfx_h.fb_height * gfx_h.fb_pitch);
}

struct display get_active();
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

/* This has no practical use, it is just a reminder for me on how I will tackle double buffering once I have a kernel heap to use more the 4k of memory (which is needed in this case) */ 
void fill_buffer(int x1, int y1, int color)
{
    uint32_t *buffer = (uint32_t*)0x444000; // To get this address I took the height * width * bpp, the result of this caluclation was how many bytes I need to allocate. I just looked in the memory map and found one that was big enough. Again this is just for me, and testing purposes 
    for (int y = 0; y < y1; y++)
    {
        for (int x = 0; x < x1; x++)
        {
            buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = 0xFFFFFF;
        }
    }

    memcpy32((uint32_t*)gfx_h.fb_addr, buffer, gfx_h.fb_height * gfx_h.fb_pitch);
}

/* Return the current BACK buffer */
struct display get_active()
{
    if (front.is_front)
        return back;
 
    return front;
}