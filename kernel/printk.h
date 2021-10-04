#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>

void set_console_offsets(int x, int y);
void printk(char *status, char *fmt, ...);
void putc(char c, int _x, int _y);
void puts(const char *s);

#endif // FMT_PRINTK_H