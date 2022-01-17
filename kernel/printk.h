#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>
#include <int/interrupts.h>

void printk(char *status, char *fmt, ...);
void printk_init(bool verbose_boot, boot_info_t term_info, bool is_vm);
void fmt_puts(const char *fmt, ...);
void puts(const char *s);

// Note: This should only be called when information
// must be shown, a kernel panic for example
void override_quiet_boot();

#endif // FMT_PRINTK_H