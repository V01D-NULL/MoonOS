#include "panic.h"
#include <util/ptr.h>
#include <drivers/io/serial.h>
#include <amd64/moon.h>
#include <stdarg.h>
#include <libk/kprintf.h>
#include <trace/strace.h>
#include <trace/sym.h>
#include <libk/kassert.h>
#include <mm/memdefs.h>
#include <mm/vmm.h>
#include <util/font8x16.h>
#include <libgraphics/double-buffering.h>
#include "printk.h"

static uint64_t fbaddr;
static uint16_t fbpitch;

void panic_init(uint64_t fb, uint16_t pitch)
{
	fbaddr = fb;
	fbpitch = pitch;
}

__no_return panic(const char *fmt, ...)
{
	if (!is_verbose_boot())
		__asm__("int $48"); //Switch to verbose boot

	uint32_t *ptr = (uint32_t*)fbaddr;
	for (int i = 0; i < 1000; i++)
	{
		ptr[1 * (fbpitch / sizeof (uint32_t)) + i] = 0xFF0000;
	}
	for(;;);
	va_list ap;
	va_start(ap, fmt);
	char panic_buff[512];
	vsnprintf(GENERIC_CAST(char *, &panic_buff), GENERIC_CAST(size_t, -1), fmt, ap);
	va_end(ap);

	printk("panic", "\n\tA kernel panic has occurred\n\t*** Reason: %s ***\n", panic_buff);
	debug(false, "Panic: %s\n", panic_buff);
	
	struct stacktrace_result res = backtrace_stack(10);
	for (int i = 0; i < res.count; i++)
	{
		backtrace_symbol(res.trace_results[i].address);
	}

	for (;;)
		;
}

void create_safe_panic_area()
{
	int64_t panic = to_virt(from_phys_higher_half(find_symbol_by_name("panic")));
	assert(panic != -1);
	vmm_remap(panic, panic, FLAGS_PR);
}