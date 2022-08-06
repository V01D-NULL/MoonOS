#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <base/base-types.h>
#include <moon-io/serial.h>

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
void printk_init(bool verbose_boot);
void puts(string_view fmt, ...);

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot();

#endif // FMT_PRINTK_H