#include "vbe.h"
#include <util/common.h>
#include <amd64/validity.h>

#define SSFN_CONSOLEBITMAP_TRUECOLOR        /* use the special renderer for 32 bit truecolor packed pixels */
#define SSFN_NOIMPLEMENTATION               /* don't include the normal renderer implementation */
#include <3rdParty/ssfn.h>

__export uint8_t _binary_font_console_sfn_start;
__export uint8_t _binary_font_console_sfn_size;
__export uint8_t _binary_font_console_sfn_end;

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

    ssfn_src =
    (ssfn_font_t*)&_binary_font_console_sfn_start;   /* the bitmap font to use */

    ssfn_dst.ptr = (uint8_t*)boot.vesa.fb_addr; /* address of the linear frame buffer */
    ssfn_dst.w = boot.vesa.fb_width;            /* width */
    ssfn_dst.h = boot.vesa.fb_height;           /* height */
    ssfn_dst.p = boot.vesa.fb_pitch;            /* bytes per line */
    ssfn_dst.x = ssfn_dst.y = 0;                /* pen position */
    ssfn_dst.fg = fg;                   /* foreground color */
    ssfn_dst.bg = bg;   //TODO: Make sure upper bits (0xff...) are 255

    gfx_h.last_known_fg = fg;
    gfx_h.last_known_bg = bg;
    bg == 0x0 ? (void)0x0 : gfx_clear(bg);
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

void gfx_set_colors(uint32_t fg, uint32_t bg)
{
    ssfn_dst.fg = fg;
    ssfn_dst.bg = bg;
}

void gfx_restore_colors()
{
    ssfn_dst.fg = gfx_h.last_known_fg;
    ssfn_dst.bg = gfx_h.last_known_bg;
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