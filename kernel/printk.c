#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include <drivers/vbe/vbe.h>
#include "printk.h"


extern gfx_header_t gfx_h;
char buffer[512];

int console_x = 0, console_y = 0;

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
    delay(20);
}

void putc(char c, int _x, int _y, bool _swap_buffers)
{
    if (c == '\n')
    {
        console_y += char_height;
        console_x = char_width - (char_width+char_width); //For some reason setting console_x = 0 causes the line to be indented by one char, this fixes it
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
    if (_swap_buffers)
        swap_buffers();
}

void puts(const char *s)
{
    for (int i = 0, n = strlen(s); i < n; i++)
    {
        putc(s[i], console_x, console_y, false);
        console_x += char_width;
    }
    swap_buffers();
}