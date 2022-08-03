#include "early_term.h"
#include <base/string.h>
#include <base/fmt.h>
#include <stdarg.h>

void (*term_write)(string_view string, size_t length);

static uint64_t term_write_paddr = 0x0;
static bool fterm_available = true;
static int fterm_width = 0;
static int fterm_height = 0;

void boot_term_init(uint64_t term_write_addr, int width, int height)
{
    term_write_paddr = term_write_addr;
    void *term_write_ptr = (void *)term_write_addr;
    term_write = term_write_ptr;
    fterm_width = width;
    fterm_height = height;
}

static char buffer[512];
void boot_term_write(string_view fmt, ...)
{
    if (!fterm_available)
        return;

    va_list arg;
    va_start(arg, fmt);
    vsnprintf((string )&buffer, (size_t)-1, fmt, arg);
    va_end(arg);

    term_write((string_view )&buffer, strlen(buffer));
}

void set_boot_term_available(bool availability)
{
    fterm_available = availability;
}

bool is_boot_term_available(void)
{
    return fterm_available;
}