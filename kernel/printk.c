#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include <libgraphics/bootsplash.h>
#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <stdarg.h>
#include "printk.h"
#include <boot/bootloader_stivale2.h>
#include <devices/serial/serial.h>
#include <util/ptr.h>

typedef struct {
    char *text;
    uint32_t x;
    uint32_t y;
    uint32_t fg;
    uint32_t bg;
    uint32_t width;
    uint32_t height;
    uint32_t offset;
    bool override_bound_checking; // scroll() needs to enable this option to avoid becoming an infinite recursive function
} fb_out_t;

char buffer[512];

static fb_out_t fb_out;
static bool is_verbose_boot = false;
extern gfx_header_t gfx_h;
uint32_t *fb_addr;

void puts(const char *s);

void printk_init(bool verbose_boot)
{
    is_verbose_boot = verbose_boot;

    // Init this even when verbose_boot is false since quiet boot can be overridden
    fb_addr = (uint32_t*)gfx_h.fb_addr;
    fb_out.text = (char*)double_buffering_create_buffer();
    fb_out.width = gfx_h.fb_width / char_width;
    fb_out.height = gfx_h.fb_height / char_height;
    fb_out.bg = 0;
    fb_out.fg = 0xFFFFFF;
    fb_out.offset = 0;
    fb_out.override_bound_checking = false;
}

void printk(char *status, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    if (!is_verbose_boot) {
        serial_set_color(BASH_GREEN);
        debug(false, "DEBUG: ");
        serial_set_color(BASH_DEFAULT);
        debug(false, "[%s] %s", status, buffer);
        return;
    }

    puts("[ ");
    puts(status);
    puts(" ] ");
    puts((const char *)&buffer);
}

__attribute__((always_inline)) static inline void plot_pix_fb(uint32_t hex, int x, int y) {
    fb_addr[y * (gfx_h.fb_pitch / sizeof(uint32_t)) + x] = hex;
}

void putc(char c, int x, int y)
{
    if (!fb_out.override_bound_checking)
        fb_out.text[y * fb_out.width + x] = c;

    if (c == '\n') {
        fb_out.y++;
        fb_out.x = 0;
        if (fb_out.y >= fb_out.height && !fb_out.override_bound_checking) {
            scroll();
        }
        return;
    }

    for (int _y = 0; _y < char_height; _y++)
    {
        for (int _x = 0; _x < char_width; _x++)
        {
            bool draw_fg = font[(c * char_height) + _y] & (1 << _x);
            int ypos = (y * char_height) + _y;
            int xpos = (x * char_width) + char_width - _x;

            plot_pix_fb(draw_fg ? fb_out.fg : fb_out.bg, xpos, ypos);
        }
    }
    fb_out.x++;
}

void puts(const char *s)
{
    for (int i = 0, n = strlen(s); i < n; i++) {
        putc(s[i], fb_out.x, fb_out.y);
    }
}

void fast_clear(void)
{
    for (int i = 0; i < gfx_h.fb_height * gfx_h.fb_width; i++)
    {
        plot_pix_fb(fb_out.bg, i % gfx_h.fb_width, i / gfx_h.fb_width);
    }
}

void scroll()
{
    size_t offset = parse_string_until_newline(fb_out.text);
    memcpy(
        GENERIC_CAST(uint8_t*, fb_out.text),
        GENERIC_CAST(uint8_t*, fb_out.text + fb_out.width),
        fb_out.width * (fb_out.height-1) * gfx_h.fb_bpp
    );
    // memset((void*)(fb_out.text + (fb_out.width * fb_out.height)), 0, fb_out.width);

    fb_out.x = fb_out.y = 0;
    fb_out.offset = 0;
    fb_out.override_bound_checking = true;
    
    for (int y = 0; y < fb_out.height; y++)
    {
        for (int x = 0; x < fb_out.width; x++)
        {
            char c = fb_out.text[y * fb_out.width + x];
            putc(c, x, y);
        }
    }

    fb_out.override_bound_checking = false;
}

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }