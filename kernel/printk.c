#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <stdarg.h>
#include "printk.h"

/* 
    Text needs to be written into a buffer if false, and then displayed to the screen. 
    A `manual_update_buffer()` function could memcpy the data in the double bufferers back buffer
    which would be useful for this scenario. After updating the buffer the buffer can just be swapped

    Or just write a verbose_boot() that will:
    
    verbose_boot(int *text_buffer):
        memset(back_buffer, 0, display_size);               // This removes the bootsplash
        memcpy(framebuffer, text_buffer, display_size);
*/
static bool verbose_boot = false;
extern gfx_header_t gfx_h;
char buffer[512];

bool is_verbose_boot()
{
    return verbose_boot;
}

void set_verbose_boot(bool new)
{
    verbose_boot = new;
}

uint32_t console_x = 0, console_y = 0, kernel_log_color = 0xFFFFFF;

uint32_t set_console_color(uint32_t new)
{
    uint32_t old_color = kernel_log_color;
    kernel_log_color = new;
    return old_color;
}

void set_console_offsets(int x, int y)
{
    if (x == -1)
    {
        console_y = y * char_height;
    }
    else if (y == -1)
    {
        console_x = x * char_width;
    }
    else
    {
        console_x = x * char_width;
        console_y = y * char_height;
    }
}

void printk(char *status, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    if (verbose_boot == true)
    {
        puts("[ ");
        uint32_t old = set_console_color(0xD2D2D2);
        puts(status);
        set_console_color(old);
        puts(" ] ");
        puts((const char *)&buffer);
    }
}

void putc(char c, int _x, int _y)
{
    if (c == '\n')
    {
        console_y += char_height;
        console_x = -char_width; /* 0 adds one character offset thus indenting strings by one character, the true offset 0 is actually negative char_width */
        goto end;
    }

    for (int y = 0; y < char_height; y++)
    {
        for (int x = 0; x < char_width; x++)
        {
            if ((font[(c * char_height) + y]) & (1 << x))
            {
                buffer_pixel(_x + char_width - x, _y + y, kernel_log_color);
            }
        }
    }

end:
    return;
}

void puts(const char *s)
{
    for (int i = 0, n = strlen(s); i < n; i++)
    {
        putc(s[i], console_x, console_y);
        console_x += char_width;
    }
    swap_buffers();
}