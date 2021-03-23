#ifndef LIB_KERNEL_STRING_H
#define LIB_KERNEL_STRING_H

#include <stdint.h>

/*
    TODO: Add strncmp
*/

void memcpy ( uint8_t *dest, const uint8_t *src, uint64_t len );

void memset ( uint8_t *dest, uint8_t val, uint64_t len );

uint64_t memcmp(const void *src, void *dst, uint64_t n);

int strcmp ( char *str1, char *str2 );

char* strcpy (char *dest, const char *src);

char* strcat(char* a, char* b);

uint64_t strlen ( const char *s );

char *strrev(char *src);

#endif // LIB_KERNEL_STRING_H