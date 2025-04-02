#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *dest, int value, size_t n);

#endif  // MEMORY_H