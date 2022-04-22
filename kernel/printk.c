#include <libk/kstring.h>
#include <libk/kprintf.h>
#include "printk.h"
#include <devices/serial/serial.h>
#include <devices/term/early/early_term.h>
#include <devices/term/limine-port/term.h>
#include <mm/mm.h>

char buffer[512];
static bool is_verbose_boot = false;

void printk_init(bool verbose_boot, BootContext term_info)
{
    set_boot_term_available(false);
    is_verbose_boot = verbose_boot;
    term_init();
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

    // _term_write(status, strlen(status));
    // _term_write(": ", 2);
    // for (size_t i = 0, n = strlen(buffer); i < n; i++)
        // _gterm_putchar(buffer[i]);

    // gterm_putchar('A');
    // gterm_double_buffer_flush();
    _term_write((const char *)&buffer, strlen(buffer));
}

void fmt_puts(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    _term_write((const char *)&buffer, strlen(buffer));
}

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }