#include <devices/term/early/early_term.h>
#include <moon-io/serial.h>
#include <base/fmt.h>
#include <moon.h>
#include <trace/sym.h>
#include <stdarg.h>
#include <mm/vmm.h>
#include <mm/mm.h>
#include "printk.h"
#include "panic.h"

NORETURN void __panic(uint64_t bp, uint64_t sp, string_view fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char panic_buff[512];
	vsnprintf((string ) &panic_buff, (size_t) -1, fmt, ap);
	va_end(ap);

	override_quiet_boot();
    
    if (is_boot_term_available())
        boot_term_write("\nA kernel panic has occurred\n*** Reason: %s ***\n", panic_buff);
	
    printk("panic", "\nA kernel panic has occurred\n");
	fmt_puts("*** Reason: %s ***\n", panic_buff);
	debug(false, "A kernel panic has occurred\n*** Reason: %s ***\n", panic_buff);
	
	struct stacktrace_result res = arch_trace_stack(10);
	for (int i = 0; i < res.count; i++)
	{
		backtrace_symbol(res.trace_results[i].address);
	}

	size_t frame_size = bp-sp;
	printk("stackdump", "Dumping %s's stackframe\nStackframe size: 0x%x\n", sym_lookup(res.trace_results[1].address).name, frame_size);
	fmt_puts("<addr>\t\t  <stack>\t   <stack+8>\n");

	// The larger the stackframe the less likely the chance of seeing messages
	// printed earlier due to the terminal scrolling. 0x18 was chosen randomly.
	if (frame_size > 0x18) frame_size = 0x18; // Todo: Write to file or something
	
	// Dump stackframe of the function that called panic()
	for (uint64_t i = 0; i < frame_size; i++)
	{
		fmt_puts("%lx: %p %p\n",
			bp, *(long*)(bp),
			*(long*)(bp + sizeof(long))
		);
		bp += sizeof(long) * 2;
	}

	for (;;)
		asm("hlt");
}