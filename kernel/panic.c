#include "panic.h"
#include <base/fmt.h>
#include <cpu.h>
#include <mm/vm_sys.h>
#include <moon-io/serial.h>
#include <moon.h>
#include <stdarg.h>
#include <trace/sym.h>
#include "printk.h"

NORETURN void __panic(size_t bp, size_t sp, string_view fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char panic_buff[512];
    vsnprintf((string)&panic_buff, (size_t)-1, fmt, ap);
    va_end(ap);

    override_quiet_boot();

    printk("panic", "\nA kernel panic has occurred\n");
    puts("*** Reason: %s ***\n", panic_buff);
    debug(
        false, "A kernel panic has occurred\n*** Reason: %s ***\n", panic_buff);

    struct stacktrace_result res = arch_trace_stack(10);
    for (int i = 0; i < res.count; i++)
    {
        backtrace_symbol(res.trace_results[i]);
    }

    size_t frame_size = bp - sp;
    printk("stackdump",
           "Dumping %s's stackframe\nStackframe size: 0x%x\n",
           sym_lookup(res.trace_results[1]).name,
           frame_size);
    puts("<addr>\t\t  <stack>\t   <stack+8>\n");

    // The larger the stackframe the less likely the chance of seeing messages
    // printed earlier due to the terminal scrolling. 0x18 was chosen randomly.
    if (frame_size > 0x18)
        frame_size = 0x18;

    // Dump stackframe of the function that called panic()
    for (size_t i = 0; i < frame_size; i++)
    {
        puts("%lx: %p %p\n",
             bp,
             *(size_t *)(bp),
             *(size_t *)(bp + sizeof(size_t)));
        bp += sizeof(size_t) * 2;
    }

    arch_halt_cpu();
    __builtin_unreachable();
}