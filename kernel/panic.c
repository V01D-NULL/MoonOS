#include "panic.h"
#include <util/ptr.h>
#include <drivers/vbe/vbe.h>
#include <drivers/io/serial.h>
#include <amd64/validity.h>
#include <stdarg.h>
#include <libk/kprintf.h>

char panic_buff[512];
__no_return panic(const char *fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(GENERIC_CAST(char *, &panic_buff), GENERIC_CAST(size_t, -1), fmt, ap);
	va_end(ap);

	printk("panic", "KERNEL PANIC: %s\n", panic_buff);
	debug(true, "KERNEL PANIC: %s\n", panic_buff);

	for (;;);
}