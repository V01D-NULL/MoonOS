#include "kassert.h"
#include "common.h"
#include "drivers/gfx/gfx.h"

void _assert(const char *file, uint64_t line, const char* func, const char *desc) {
    gfx_set_colors(0xF70D1A, 0x0);
    printk("%s:%d: %s: Assertion `%s' failed", (char*)file, line, func, desc);

    for (;;)
        asm ("hlt");
}