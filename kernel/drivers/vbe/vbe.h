/**
 * @file gfx.h
 * @author Tim (V01D)
 * @brief 
 * @version 0.1
 * @date 2021-04-20
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef GFX_H
#define GFX_H

#include <boot/bootloader_stivale2.h>
#include <libk/kprintf.h>
#include <libk/kstring.h>
#include <util/common.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdbool.h>

#define OPERATION_BUSY 0x231B //unicode hourglass
#define OPERATION_OK   0x2705 //unicode checkmark
#define OPERATION_FAIL 0x26D4 //unicode stop sign
#define GLYPH_HEIGHT   16     //Height of the fonts glyphs

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

void gfx_set_colors(uint32_t fg, uint32_t bg);
void gfx_restore_colors();
void gfx_scroll();
void gfx_vertical_scroll(int n);
void gfx_clear_x(int y1, int color);
void gfx_init(boot_info_t boot, int fg, int bg);
void put_pixel(int x, int y, int color);
void gfx_clear(int color);

#endif // GFX_H
