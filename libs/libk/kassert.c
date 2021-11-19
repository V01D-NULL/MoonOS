#include "kassert.h"
#include <devices/serial/serial.h>
#include <util/common.h>
#include <printk.h>

void __assert_impl(const char *file, uint64_t line, const char *func, const char *desc)
{
    printk("ASSERT", "%s:%d: %s: Assertion `%s' failed", (char *)file, line, func, desc);
    debug(true, "ASSERT", "%s:%d: %s: Assertion `%s' failed", (char *)file, line, func, desc);
    
    for (;;)
        __asm__("hlt");
}