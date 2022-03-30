#include "fterm.h"
#include <stddef.h>
#include <libk/kstring.h>
#include <libk/kprintf.h>
#include <stdarg.h>
#include <mm/vmm.h>
#include <devices/serial/serial.h>

void (*term_write)(const char *string, size_t length);

static uint64_t term_write_paddr = 0x0;
static bool fterm_available = true;
static int fterm_width = 0;
static int fterm_height = 0;

void fterm_init(uint64_t term_write_addr, int width, int height)
{
    term_write_paddr = term_write_addr;
    void *term_write_ptr = (void *)term_write_addr;
    term_write = term_write_ptr;
    fterm_width = width;
    fterm_height = height;
}

static char buffer[512];
void fterm_write(const char *fmt, ...)
{
    if (!fterm_available) return;

    va_list arg;
    va_start(arg, fmt);
    vsnprintf((char *)&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    term_write((char*)&buffer, strlen(buffer));
}

void set_fterm_available(bool availability)
{
    fterm_available = availability;
}

bool is_fterm_available(void)
{
    return fterm_available;
}

void fterm_flush(void)
{
    fterm_write("\033[1;1H");
    for (int i = 0; i < fterm_width * fterm_height; i++)
        fterm_write(" ");

    fterm_write("\033[1;1H");
}