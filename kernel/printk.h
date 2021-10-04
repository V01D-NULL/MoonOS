#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>

/* Set the log color to 'new', returns the previous color used */
uint32_t set_console_color(uint32_t new);
void set_console_offsets(int x, int y);
void printk(char *status, char *fmt, ...);
void putc(char c, int _x, int _y);
void puts(const char *s);

void set_verbose_boot(bool new);
bool is_verbose_boot();

#endif // FMT_PRINTK_H