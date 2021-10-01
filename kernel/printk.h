#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>

void putc(uint32_t unicode, int x, int y);
void puts(const char *str);
void printk(char *status, char *fmt, ...);
void printk_wait(bool (*callback)(), char *fmt,  ...);

#endif // FMT_PRINTK_H