#ifndef PANIC_H
#define PANIC_H

#include <amd64/moon.h>
#include <stdint.h>

// panic if eval evaluates to true
#define panic_if(eval, ...) eval ? panic(__VA_ARGS__) : (void)0
gnu_no_return panic(const char *fmt, ...);

#endif // PANIC_H