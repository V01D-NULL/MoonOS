#include "panic.h"
#include <util/ptr.h>
#include <drivers/vbe/vbe.h>
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
#include "printk.h"

__no_return panic(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	char panic_buff[512];
	vsnprintf(GENERIC_CAST(char *, &panic_buff), GENERIC_CAST(size_t, -1), fmt, ap);
	va_end(ap);

	printk("panic", "\n\tA kernel panic has occurred\n\t*** Reason: %s ***\n", panic_buff);

	struct stacktrace_result res = backtrace_stack(10);
	for (int i = 0; i < res.count; i++)
	{
		backtrace_symbol(res.trace_results[i].address);
	}

	for (;;)
		;
}

// early_panic is called before any fancy features have been intialized, most notably double buffering.
// early_panic writes directly to the framebuffer and should ONLY be called if double buffering is not enabled for whatever reason
__no_return early_panic(const char *error)
{
	/* Write pixels in the string to the framebuffer char by char */
	int console_x = 0, console_y = 1;
	for (int i = 0, n = strlen(error); i < n; i++)
	{
		for (int y = 0; y < char_height; y++)
		{
			for (int x = 0; x < char_width; x++)
			{
				if ((font[(error[i] * char_height) + y]) & (1 << x))
				{
					put_pixel(console_x + char_width - x, console_y + y, 0xFFFFFF);
				}
			}
		}
		console_x += char_width;
	}

	for (;;)
		;
}

void create_safe_panic_area()
{
	uint64_t panic = to_virt(from_phys(find_symbol_by_name("panic")));
	assert(panic != -1);
	vmm_remap(panic, panic, FLAGS_PR);
}