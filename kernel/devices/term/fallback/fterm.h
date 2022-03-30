#ifndef FALLBACK_TERM_H
#define FALLBACK_TERM_H

#include <stdint.h>
#include <stdbool.h>

void fterm_init(uint64_t term_write_addr, int width, int height);
void fterm_write(const char *fmt, ...);
void set_fterm_available(bool availability);
bool is_fterm_available(void);
void fterm_flush(void);
void fterm_map(void);

#endif // FALLBACK_TERM_H