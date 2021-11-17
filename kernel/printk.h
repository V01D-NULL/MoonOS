#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>
#include <int/interrupts.h>

/* Set the log color to 'new', returns the previous color used */
void set_console_offsets(int x, int y);
void printk(char *status, char *fmt, ...);
void putc(char c, int _x, int _y);
void puts(const char *s);
void printk_init(void);
void verbose_boot_irq(isr_t isr);
bool is_verbose_boot(void);
void scroll(void);

#endif // FMT_PRINTK_H