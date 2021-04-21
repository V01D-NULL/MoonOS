#include "gfx.h"
#include "../../common.h"

#define SSFN_CONSOLEBITMAP_TRUECOLOR        /* use the special renderer for 32 bit truecolor packed pixels */
#define SSFN_NOIMPLEMENTATION               /* don't include the normal renderer implementation */
#include <3rdParty/ssfn.h>

extern uint8_t _binary_font_console_sfn_start;
extern uint8_t _binary_font_console_sfn_size;
extern uint8_t _binary_font_console_sfn_end;

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

long char_cnt = 0;
void putc(uint32_t unicode, int x, int y)
{
    if (x != -1 || y != -1)
    {
        ssfn_dst.x = x;
        ssfn_dst.y = y;
    }
    
    ssfn_putc(unicode);
    char_cnt++;
}

void puts(const char *str)
{
    for (int i = 0, n = strlen(str); i < n; i++)
    {
        //Handle text wrapping
        if (ssfn_dst.x >= ssfn_dst.w)
        {
            ssfn_putc('\n');
        }
        ssfn_putc(str[i]);
        char_cnt++;
    }
    if (ssfn_dst.y >= gfx_h.fb_height)
    {
        gfx_scroll(1);
    }
}

char buffer[512];

//Use this when waiting for a job to complete (it's poorly coded but, meh. Writing to video memory is fast anyway it's fine)
//TODO: Create a queue.c file where a function pointer is defined.
// All queued functions will be tracked with a linked list.
// Insert the function pointer in printk_wait and once the function is done modify the hourglass to an OK or ERR (depending on what the function returns)
void printk_wait(bool (*callback)(), char *fmt,  ...)
{
    va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buffer, (size_t) -1, fmt, arg);
	va_end(arg);

    putc('[', -1, -1);
    gfx_set_colors(0xffffff, 0x0);
    putc(OPERATION_BUSY, -1, -1);
    gfx_restore_colors();
    puts("] ");
    
    puts((const char*)&buffer);
    ssfn_dst.y = ssfn_dst.y - GLYPH_HEIGHT; //Don't increment y

    //Call function pointer
    bool ret = (*callback) ();

    //Delete line with the hourglass
    gfx_clear_x(ssfn_dst.y, 0x0);
    
    //Reset x position
    ssfn_dst.x = 0;

    //Check return type (true = ok, false = err)
    if (ret == true)
    {
        putc('[', -1, -1);
        gfx_set_colors(0x4CC417, 0x0);
        putc(OPERATION_OK, -1, -1);
        gfx_restore_colors();
        puts("] ");
    }
    else
    {
        putc('[', -1, -1);
        gfx_set_colors(0xF70D1A, 0x0);
        putc(OPERATION_FAIL, -1, -1);
        gfx_restore_colors();
        puts("] ");   
    }
    puts((const char*)&buffer);
}

void printk(char *status, char *fmt, ...)
{
    va_list arg;
	va_start(arg, fmt);
	vsnprintf((char*)&buffer, (size_t) -1, fmt, arg);
	va_end(arg);

    if (strcmp(status, "OK") == 0)
    {
        putc('[', -1, -1);
        gfx_set_colors(0x4CC417, 0x0);
        putc(OPERATION_OK, -1, -1);
        gfx_restore_colors();
        puts("] ");
    }
    else if (strcmp(status, "ERR") == 0)
    {
        putc('[', -1, -1);
        gfx_set_colors(0xF70D1A, 0x0);
        putc(OPERATION_FAIL, -1, -1);
        gfx_restore_colors();
        puts("] ");
        
    }
    else
    {
        puts("[ ");
        puts(status);
        puts(" ] ");
    }
    puts((const char*)&buffer);

    delay(20);
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