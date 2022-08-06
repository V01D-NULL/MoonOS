#include <devices/term/tty.h>
#include <moon-io/serial.h>
#include <base/string.h>
#include <base/fmt.h>
#include <cpu.h>
#include "printk.h"

static bool is_verbose_boot = false;

void printk_init(bool verbose_boot)
{
    arch_tty_init();
    is_verbose_boot = verbose_boot;
}

void printk(string status, string fmt, ...)
{
	if (!is_verbose_boot)
		return;
	
    char buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string )&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

	puts("[core#%d] %s: %s", current_cpu(), status, buffer);
}

void puts(string_view fmt, ...)
{
    char buffer[512];
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

	arch_tty_write((string_view)&buffer);
}

// Note: This should only be called when information
// must be shown. A kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }