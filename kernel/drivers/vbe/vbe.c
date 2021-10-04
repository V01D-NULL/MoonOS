#include "vbe.h"
#include <util/common.h>
#include <amd64/moon.h>
#include <drivers/io/serial.h>

gfx_header_t gfx_h;
uint32_t *framebuffer_address;

void gfx_init(boot_info_t boot, int fg, int bg)
{
    gfx_h.fb_addr = boot.vesa.fb_addr;
    gfx_h.fb_bpp  = boot.vesa.fb_bpp;
    gfx_h.fb_height = boot.vesa.fb_height;
    gfx_h.fb_width = boot.vesa.fb_width;
    gfx_h.fb_pitch = boot.vesa.fb_pitch;

    framebuffer_address = (uint32_t*) gfx_h.fb_addr;

    gfx_h.last_known_fg = fg;
    gfx_h.last_known_bg = bg;
    bg == 0x0 ? (void)0x0 : gfx_clear(bg);
    debug(0, "%d %d\n", boot.vesa.fb_width, boot.vesa.fb_height);
}

void put_pixel(int x, int y, int color)
{
    // index = y * (pitch / sizeof(uint32_t)) + x
    framebuffer_address[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = color;
}

void gfx_clear(int color)
{
    for (int y = 0; y < gfx_h.fb_height; y++)
    {
        for (int x = 0; x < gfx_h.fb_width; x++)
        {
            put_pixel(x, y, color);
        }
    }
}

void gfx_clear_x(int y1, int color)
{
    for (int y = y1; y < y1 + GLYPH_HEIGHT; y++)
    {
        for (int x = 0; x < gfx_h.fb_width; x++)
        {
            put_pixel(x, y, color);
            // debug("(%d, %d)\n", x, y);
        }
    }
}

//Move everything up by 1 line
void gfx_scroll()
{
    memmove((void*)gfx_h.fb_addr, (void *)((size_t)gfx_h.fb_addr + gfx_h.fb_width * GLYPH_HEIGHT * 4), gfx_h.fb_width * (gfx_h.fb_height - GLYPH_HEIGHT) * 4);
}

void gfx_vertical_scroll(int n)
{
    uint8_t *fb = (uint8_t*) gfx_h.fb_addr;

    for (int j = 0; j < n ; j++)
        for (int i = 0; i < (gfx_h.fb_height * gfx_h.fb_width) + gfx_h.fb_height; i++)
        {
            fb[i] = fb[i + 80];
        }
}