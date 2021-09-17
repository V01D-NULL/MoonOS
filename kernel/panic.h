#ifndef PANIC_H
#define PANIC_H

void create_safe_panic_area();
void panic(const char *fmt, ...);

#endif // PANIC_H