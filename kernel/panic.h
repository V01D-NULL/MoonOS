#ifndef PANIC_H
#define PANIC_H

#include <amd64/moon.h>
#include <stdint.h>
void create_safe_panic_area();
__no_return panic(const char *fmt, ...);
void panic_init(uint64_t fb, uint16_t pitch);

#endif // PANIC_H