#include "double-buffering.h"
#include <libk/kstring.h>
#include <mm/mm.h>
#include <devices/fb/early_fb.h>

static uint32_t *buffer;
gfx_header_t gfx_h;

void double_buffering_init(BootContext boot)
{
    gfx_h.fb_addr = boot.fb.fb_addr - $high_vma;
    gfx_h.fb_bpp = boot.fb.fb_bpp;
    gfx_h.fb_height = boot.fb.fb_height;
    gfx_h.fb_width = boot.fb.fb_width;
    gfx_h.fb_pitch = boot.fb.fb_pitch;

    buffer = (uint32_t *)fb_1080x1920_screen_malloc(gfx_h.fb_width * gfx_h.fb_height * (gfx_h.fb_bpp / 8));
    flush_back_buffer(NULL);
}

void swap_buffers(uint32_t *custom_buffer)
{
    if (custom_buffer == NULL)
        memcpy32((uint32_t *)gfx_h.fb_addr, buffer, gfx_h.fb_height * gfx_h.fb_pitch);
    else
        memmove((uint32_t *)gfx_h.fb_addr, custom_buffer, gfx_h.fb_height * gfx_h.fb_pitch);
}

void buffer_pixel(int x, int y, int color)
{
    buffer[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
}

void flush_back_buffer(uint32_t *custom_buffer)
{
    if (custom_buffer == NULL)
    {
        memset(buffer, 0, gfx_h.fb_height * gfx_h.fb_pitch);
    }
    else
    {
        memset(custom_buffer, 0, gfx_h.fb_height * gfx_h.fb_pitch);
    }
}

// Todo: Need Heap
uint32_t *double_buffering_create_buffer(void)
{
    // return (uint32_t *)heap_alloc(gfx_h.fb_width * gfx_h.fb_height * (gfx_h.fb_bpp / 8)).base;
    return NULL;
}
