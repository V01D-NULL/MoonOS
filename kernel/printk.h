#ifndef FMT_PRINTK_H
#define FMT_PRINTK_H

#include <stdint.h>
#include <stdbool.h>

void printk(char *status, char *fmt, ...);
void printk_wait(bool (*callback)(), char *fmt,  ...);
void putc(char c, int _x, int _y, bool _swap_buffers);
void puts(const char *s);
void kernel_log_init();

#endif // FMT_PRINTK_H