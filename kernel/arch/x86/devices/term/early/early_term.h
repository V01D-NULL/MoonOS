#ifndef EARLY_BOOT_TERM_H
#define EARLY_BOOT_TERM_H

#include <base/base-types.h>

void boot_term_init(uint64_t term_write_addr, int width, int height);
void boot_term_write(string_view fmt, ...);
void set_boot_term_available(bool availability);
bool is_boot_term_available(void);

#endif // EARLY_BOOT_TERM_H