#include <libk/kstring.h>
#include <libk/kprintf.h>
#include "printk.h"
#include <boot/bootloader_stivale2.h>
#include <devices/serial/serial.h>

char buffer[512];
static bool is_verbose_boot = false;

void puts(const char *s);
void (*term_write)(const char *string, size_t length);

void printk_init(bool verbose_boot, struct stivale2_struct_tag_terminal *term)
{
    is_verbose_boot = verbose_boot;

    void *term_write_ptr = (void *)term->term_write;
    term_write = term_write_ptr;
}

void printk(char *status, char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    if (!is_verbose_boot) {
        serial_set_color(BASH_GREEN);
        debug(false, "DEBUG: ");
        serial_set_color(BASH_DEFAULT);
        debug(false, "[%s] %s", status, buffer);
        return;
    }

    puts("\033[0;37m");
    puts(status);
    puts("\033[1;30m" ": ");
    puts((const char *)&buffer);
}

void puts(const char *s)
{
    term_write(s, strlen(s));
}

void fmt_puts(const char *fmt, ...)
{
    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    puts((const char*)&buffer);
}

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot() { is_verbose_boot = true; }