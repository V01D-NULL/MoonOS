#include <devices/term/early/early_term.h>
#include <devices/term/limine-port/term.h>
#include <moon-io/serial.h>
#include <base/string.h>
#include <base/fmt.h>
#include <amd64/cpu.h>
#include <mm/mm.h>
#include "printk.h"

static bool is_verbose_boot = false;

void printk_init(bool verbose_boot, BootContext term_info)
{
    set_boot_term_available(false);
    is_verbose_boot = verbose_boot;
    term_init();
}

void printk(string status, string fmt, ...)
{
    char buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string )&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    fmt_puts("[core#%d] %s: %s", current_cpu(), status, buffer);
}

void fmt_puts(string_view fmt, ...)
{
    char buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string )&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    if (!is_verbose_boot)
    {
        debug(false, BASH_GREEN "DEBUG: " BASH_DEFAULT "%s", (string_view)&buffer);
        return;
    }

    _term_write((string_view)&buffer, strlen(buffer));
}

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }