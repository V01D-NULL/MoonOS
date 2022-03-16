#include <libk/kstring.h>
#include <libk/kprintf.h>
#include "printk.h"
#include <boot/bootloader_stivale2.h>
#include <devices/serial/serial.h>
#include <devices/term/fallback/fterm.h>
#include <devices/term/vt100/vtconsole.h>
#include <util/font8x16.h>
#include <mm/mm.h>

#define EARLY_CON_MAX_SCREEN_SIZE 1920 * 1080

char buffer[512];
static bool is_verbose_boot = false;
static bool use_fallback = true;

int width = 0, height = 0;
uint32_t *fb = NULL;
vtconsole_t *vtc;

void putc(wchar_t c, int x, int y);

void vt100_on_paint(vtconsole_t *vtc, vtcell_t *cell, int x, int y)
{
    putc(cell->c, x, y);
}

void printk_init(bool verbose_boot, boot_info_t term_info, bool is_vm)
{
    is_verbose_boot = verbose_boot;
    uint64_t scr_dimensions = term_info.vesa.fb_width * term_info.vesa.fb_height;
    width = term_info.vesa.fb_width;
    height = term_info.vesa.fb_height;
    fb = (uint32_t *)(term_info.vesa.fb_addr);

    // Don't start early term (scrolling is slow on hw), just use the fallback one.
    if (!is_vm)
        return;

    // Make sure we can allocate a buffer for the vtconsole, if not we need to wait for the heap to become available.
    if (scr_dimensions <= EARLY_CON_MAX_SCREEN_SIZE)
    {
        // Half the display = double the scrolling speed on hardware, but it's still not that fast.
        // Not sure if I like this approach.
        // for (int i = 0; i < term_info.vesa.fb_height * term_info.vesa.fb_width; i++)
        //     plot_pix_fb(0x0, i % term_info.vesa.fb_width, i / term_info.vesa.fb_width);

        // for (int x = 0; x < term_info.vesa.fb_width; x++)
        //     plot_pix_fb(0xFFFFFF, x, term_info.vesa.fb_height / 2);

        // use_fallback = false;
        // vtc = vtconsole(term_info.vesa.fb_width / char_width, (term_info.vesa.fb_height / 2) / char_height, vt100_on_paint, NULL);
        
        use_fallback = false;
        vtc = vtconsole(term_info.vesa.fb_width / char_width, term_info.vesa.fb_height / char_height, vt100_on_paint, NULL, is_verbose_boot);
    }
}

void printk(char *status, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    if (!is_verbose_boot)
    {
        serial_set_color(BASH_GREEN);
        debug(false, "DEBUG: ");
        serial_set_color(BASH_DEFAULT);
        debug(false, "[%s] %s", status, buffer);
        return;
    }

    puts((const char *)&buffer);
}

void puts(const char *s)
{
    if (use_fallback)
    {
        fterm_write(s);
    }
    else
    {
        vtconsole_write(vtc, s, strlen(s));
    }
}

void fmt_puts(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    puts((const char *)&buffer);
}

void plot_pix_fb(uint32_t hex, int x, int y)
{
    fb[x + y * width] = hex;
}

void putc(wchar_t c, int x, int y)
{
    for (int _y = 0; _y < char_height; _y++)
    {
        for (int _x = 0; _x < char_width; _x++)
        {
            bool draw_fg = font[(c * char_height) + _y] & (1 << _x);
            int ypos = (y * char_height) + _y;
            int xpos = (x * char_width) + char_width - _x;

            // #cdcdcd is a light gray-ish color
            plot_pix_fb(draw_fg ? 0xCDCDCD : 0x0, xpos, ypos);
        }
    }
}

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }