#define PR_MODULE "assert"
#include "assert.h"
#include <cpu.h>
#include <moon-io/serial.h>
#include <printk.h>

void __assert_impl(string_view file, uint64_t line, string_view func,
                   string_view desc)
{
    trace(TRACE_ERROR,
          "%s:%d: %s: Assertion `%s' failed",
          (char *)file,
          line,
          func,
          desc);

    debug(true,
          "ASSERT: %s:%d: %s: Assertion `%s' failed",
          (char *)file,
          line,
          func,
          desc);

    arch_halt_cpu();
}