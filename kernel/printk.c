#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include <libgraphics/bootsplash.h>
#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <stdarg.h>
#include "printk.h"
#include <boot/bootloader_stivale2.h>
#include <drivers/io/serial.h>

uint32_t console_x = 0, console_y = 0, kernel_log_color = 0xFFFFFF;
static bool verbose_boot = false;
char buffer[512];
uint32_t *boot_log_buffer;
char *text_buffer;
int boot_log_offset = 0;
extern gfx_header_t gfx_h;

void text2buffer();

void printk_init()
{
    boot_log_buffer = double_buffering_create_buffer();
    text_buffer     = (char*)double_buffering_create_buffer();
}

void verbose_boot_irq(isr_t isr)
{
    if (!verbose_boot)
    {
        flush_back_buffer(boot_log_buffer);
        text2buffer();
        swap_buffers(boot_log_buffer);
    }
    else
    {
        bootsplash();
    }

    verbose_boot = !verbose_boot;
}

bool is_verbose_boot()
{
    return verbose_boot;
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

    if (console_y >= (gfx_h.fb_height - char_height))
    {
        scroll();
    }
}

void putc(char c, int _x, int _y)
{
    text_buffer[boot_log_offset++] = c;
    
    if (c == '\n')
    {
        console_y += char_height;
        console_x = -char_width; /* 0 adds one character offset thus indenting strings by one character, the true offset 0 is actually negative char_width */
        goto end;
    }

    text2buffer();

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
    if (verbose_boot)
        swap_buffers(boot_log_buffer);
}

// Convert text data (strings) to raw pixels in the back buffer so that they can be displayed to the screen
void text2buffer()
{
    int _x = 0, _y = 0;
    for (int i = 0; i < boot_log_offset; i++)
    {
        char c = text_buffer[i];

        if (c == '\n')
        {
            _y += char_height;
            _x = -char_width;
        }
        for (int y = 0; y < char_height; y++)
        {
            for (int x = 0; x < char_width; x++)
            {
                if ((font[(c * char_height) + y]) & (1 << x))
                {
                    int ypos = _y + y;
                    int xpos = _x + char_width - x;

                    boot_log_buffer[ypos * (gfx_h.fb_pitch / sizeof(uint32_t)) + xpos] = kernel_log_color;
                }
            }
        }
        _x += char_width;
    }
}

void scroll()
{
    int offset = parse_string_until_newline(text_buffer);
    
    for (int i = offset, k = 0, buff_len = strlen(text_buffer)-1; i <= buff_len; i++, k++)
    {
        text_buffer[k] = text_buffer[i];
    }

    //Clear out left over chars
    int base = strlen(text_buffer) - offset - 1;
    for (int i = 0; i < offset; i++)
    {
        text_buffer[base + i] = ' ';
    }

    flush_back_buffer(boot_log_buffer);
    console_x = -char_width;
    console_y = gfx_h.fb_height - char_height;
    text2buffer();
}