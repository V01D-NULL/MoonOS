#include "kassert.h"
#include "common.h"
#include "drivers/vga/vga.h"

void _assert(const char *file, uint64_t line, const char* func, const char *desc) {
    set_color(VGA_BLACK, VGA_LIGHT_RED);
    kprintf("%s:%d: %s: Assertion `%s' failed", file, line, func, desc);

    for (;;)
        asm ("hlt");
}