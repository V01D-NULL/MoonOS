#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <ktypes.h>
#include <boot/boot.h>
#include <devices/serial/serial.h>

#ifndef PR_MODULE
#define PR_MODULE __FILE__
#endif

#define TODO(x, ...)                    \
	do                                  \
	{                                   \
		printk("TODO", x, __VA_ARGS__); \
		debug(true, x, __VA_ARGS__);    \
	} while (0)

#define trace(...)                      \
	do                                  \
	{                                   \
		printk(PR_MODULE, __VA_ARGS__); \
	} while (0)

void printk(string status, string fmt, ...);
void printk_init(bool verbose_boot, BootContext term_info);
void fmt_puts(string_view fmt, ...);
void puts(string_view s);

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot();

#endif // FMT_PRINTK_H