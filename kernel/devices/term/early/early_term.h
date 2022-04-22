#ifndef EARLY_BOOT_TERM_H
#define EARLY_BOOT_TERM_H

#include <stdint.h>
#include <stdbool.h>

void boot_term_init(uint64_t term_write_addr, int width, int height);
void boot_term_write(const char *fmt, ...);
void set_boot_term_available(bool availability);
bool is_boot_term_available(void);

#endif // EARLY_BOOT_TERM_H