#include "printk.h"
#include <base/fmt.h>
#include <base/string.h>
#include <cpu.h>
#include <devices/tty.h>
#include <moon-io/serial.h>

static bool is_verbose_boot = false;

void printk_init(bool verbose_boot, uint64_t fb_addr, uint16_t fb_width,
                 uint16_t fb_height, uint16_t fb_pitch,
                 uint8_t fb_red_mask_size, uint8_t fb_red_mask_shift,
                 uint8_t fb_green_mask_size, uint8_t fb_green_mask_shift,
                 uint8_t fb_blue_mask_size, uint8_t fb_blue_mask_shift)
{
    arch_tty_init(fb_addr,
                  fb_width,
                  fb_height,
                  fb_pitch,
                  fb_red_mask_size,
                  fb_red_mask_shift,
                  fb_green_mask_size,
                  fb_green_mask_shift,
                  fb_blue_mask_size,
                  fb_blue_mask_shift);
    is_verbose_boot = verbose_boot;
}

void printk(string status, string fmt, ...)
{
    if (!is_verbose_boot)
        return;

    char    buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    puts("[core#%d] %s: %s", current_cpu(), status, buffer);
}

void puts(string_view fmt, ...)
{
    char    buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    arch_tty_write((string_view)&buffer);
}

// Note: This should only be called when information
// must be shown. A kernel panic for example
void override_quiet_boot()
{
    is_verbose_boot = true;
}