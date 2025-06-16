#ifndef MEMORY_H
#define MEMORY_H

#include <stddef.h>
#include <stdint.h>

void    *memcpy(void *dest, const void *src, size_t n);
void    *memset(void *dest, int value, size_t n);
uint64_t memcmp(const void *src, void *dst, uint64_t n);

#endif  // MEMORY_H