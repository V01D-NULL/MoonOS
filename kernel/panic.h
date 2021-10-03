#ifndef PANIC_H
#define PANIC_H

#include <amd64/validity.h>

void create_safe_panic_area();
__no_return panic(const char *fmt, ...);
__no_return early_panic(const char *error);

#endif // PANIC_H