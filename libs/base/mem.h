#ifndef BASE_MEM_H
#define BASE_MEM_H

#include "base-types.h"

void memcpy(uint8_t *dest, const uint8_t *src, uint64_t len);
void memcpy32(uint32_t *dest, const uint32_t *src, uint64_t len);
void memcpy64(uint64_t *dest, const uint64_t *src, uint64_t len);

void *memset (void *dest, const uint8_t val, size_t len);
uint64_t memcmp(const void *src, void *dst, uint64_t n);
void *memmove(void *dst, const void *src, size_t n);

#endif