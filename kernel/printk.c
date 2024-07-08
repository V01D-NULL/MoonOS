#include "printk.h"
#include <base/fmt.h>
#include <base/string.h>
#include <cpu.h>
#include <devices/tty.h>
#include <moon-io/serial.h>

static bool is_verbose_boot = false;

void printk_init(bool verbose_boot, HandoverFramebuffer fb)
{
    arch_tty_init(fb.address,
                  fb.width,
                  fb.height,
                  fb.pitch,
                  fb.red_mask_size,
                  fb.red_mask_shift,
                  fb.green_mask_size,
                  fb.green_mask_shift,
                  fb.blue_mask_size,
                  fb.blue_mask_shift);

    is_verbose_boot = verbose_boot;
}

void printk(string_view status, string_view fmt, ...)
{
    if (unlikely(!is_verbose_boot))
        return;

    char    buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    tty_write("[core#%d] %s: %s", current_cpu(), status, buffer);
}

void write(string_view fmt, ...)
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
void override_quiet_boot(void)
{
    is_verbose_boot = true;
}