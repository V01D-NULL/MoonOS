#ifndef LIB_KERNEL_STRING_H
#define LIB_KERNEL_STRING_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void memcpy(uint8_t *dest, const uint8_t *src, uint64_t len);
void memcpy32(uint32_t *dest, const uint32_t *src, uint64_t len);
void memcpy64(uint64_t *dest, const uint64_t *src, uint64_t len);

void *memset (void *dest, const uint8_t val, size_t len);
uint64_t memcmp(const void *src, void *dst, uint64_t n);
void *memmove(void *dst, const void *src, size_t n);

int strcmp(const char *str1, const char *str2);
int strncmp(const char *str1, const char *str2, size_t n);
char* strcpy(char *dest, const char *src);
char* strcat(char* a, char* b);
uint64_t strlen(const char *s);
char *strrev(char *src);

bool isdigit(int c);

#endif // LIB_KERNEL_STRING_H