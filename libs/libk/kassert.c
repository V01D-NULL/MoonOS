#include "kassert.h"
#include <util/common.h>
#include <printk.h>
#include <drivers/vbe/vbe.h>

void __assert_impl(const char *file, uint64_t line, const char *func, const char *desc)
{
    printk("ASSERT", "%s:%d: %s: Assertion `%s' failed", (char *)file, line, func, desc);

    for (;;)
        __asm__("hlt");
}