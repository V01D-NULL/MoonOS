#include "kassert.h"
#include "util/common.h"
#include "drivers/gfx/gfx.h"

void __assert_impl(const char *file, uint64_t line, const char* func, const char *desc) {
    gfx_set_colors(0xF70D1A, 0x0);
    printk("ASSERT", "%s:%d: %s: Assertion `%s' failed", (char*)file, line, func, desc);

    for (;;)
        __asm__ ("hlt");
}