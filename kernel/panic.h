#ifndef PANIC_H
#define PANIC_H

#include <amd64/moon.h>
#include <stdint.h>

__no_return panic(const char *fmt, ...);

#endif // PANIC_H