#include "panic.h"
#include <util/ptr.h>
#include <devices/serial/serial.h>
#include <amd64/moon.h>
#include <stdarg.h>
#include <libk/kprintf.h>
#include <trace/strace.h>
#include <trace/sym.h>
#include <libk/kassert.h>
#include <mm/mm.h>
#include <mm/vmm.h>
#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include "printk.h"


gnu_no_return void _panic(uint64_t rbp, uint64_t rsp, const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char panic_buff[512];
	vsnprintf(GENERIC_CAST(char *, &panic_buff), GENERIC_CAST(size_t, -1), fmt, ap);
	va_end(ap);

	override_quiet_boot();

	printk("panic", "\n" BASH_RED "A kernel panic has occurred\n" BASH_WHITE);
	fmt_puts("*** Reason: %s***\n", panic_buff);
	debug(false, "A kernel panic has occurred\n*** Reason: %s ***\n", panic_buff);
	
	struct stacktrace_result res = backtrace_stack(10);
	for (int i = 0; i < res.count; i++)
	{
		backtrace_symbol(res.trace_results[i].address);
	}

	size_t frame_size = rbp-rsp;
	printk("stackdump", "Dumping %s's stackframe\nStackframe size: 0x%x\n", sym_lookup(res.trace_results[1].address).name, frame_size);
	fmt_puts("<addr>\t\t  <stack>\t   <stack+8>\n");

	// The larger the stackframe the less likely the chance of seeing messages
	// printed earlier due to the terminal scrolling. 0x18 was chosen randomly.
	if (frame_size > 0x18) frame_size = 0x18; // Todo: Write to file or something
	
	// Dump stackframe of the function that called panic()
	for (uint64_t i = 0; i < frame_size; i++)
	{
		fmt_puts("%lx: %p %p\n",
			rbp, *(long*)(rbp),
			*(long*)(rbp + sizeof(long))
		);
		rbp += sizeof(long) * 2;
	}

	for (;;)
		asm("hlt");
}