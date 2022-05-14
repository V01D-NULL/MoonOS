#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>
#include <boot/boot.h>

#ifndef PR_MODULE
#define PR_MODULE "anonymous"
#endif

#define TODO(x, ...)                    \
    do                                  \
    {                                   \
        printk("TODO", x, __VA_ARGS__); \
        debug(true, x, __VA_ARGS__);    \
    } while (0)

#define pr_info(...) printk(PR_MODULE, __VA_ARGS__)

void printk(char *status, char *fmt, ...);
void printk_init(bool verbose_boot, BootContext term_info);
void fmt_puts(const char *fmt, ...);
void puts(const char *s);

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot();

#endif // FMT_PRINTK_H