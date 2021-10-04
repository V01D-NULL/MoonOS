#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include <drivers/vbe/vbe.h>
#include "printk.h"

extern gfx_header_t gfx_h;
char buffer[512];

int console_x = 0, console_y = 0;

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

    puts("[ ");
    puts(status);
    puts(" ] ");

    puts((const char *)&buffer);
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
                buffer_pixel(_x + char_width - x, _y + y, 0xFFFFFF);
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