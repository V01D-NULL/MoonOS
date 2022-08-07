#include "assert.h"
#include <moon-io/serial.h>
#include <printk.h>

void __assert_impl(string_view file, uint64_t line, string_view func, string_view desc)
{
    printk("ASSERT", "%s:%d: %s: Assertion `%s' failed", (char *)file, line, func, desc);
    debug(true, "ASSERT", "%s:%d: %s: Assertion `%s' failed", (char *)file, line, func, desc);
    
    for (;;)
        asm("wfi");
}