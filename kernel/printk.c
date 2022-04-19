#include <libk/kstring.h>
#include <libk/kprintf.h>
#include "printk.h"
#include <devices/serial/serial.h>
#include <devices/term/fallback/fterm.h>
#include <devices/term/vt100/vtconsole.h>
#include <mm/mm.h>

char buffer[512];
static bool is_verbose_boot = false;

void printk_init(bool verbose_boot, BootContext term_info)
{
    is_verbose_boot = verbose_boot && is_fterm_available();
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

    fterm_write(BASH_CYAN);
    fterm_write(status);
    fterm_write(": " BASH_WHITE);
    fterm_write((const char *)&buffer);
}

void fmt_puts(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    fterm_write((const char *)&buffer);
}

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }