#ifndef PANIC_H
#define PANIC_H

#include <amd64/moon.h>

void create_safe_panic_area();
__no_return panic(const char *fmt, ...);

#endif // PANIC_H