#include <stddef.h>
#include <stdint.h>

void *memset(void *dest, int value, size_t n)
{
    uint8_t *ptr = dest;
    while (n--)
    {
        *ptr++ = (uint8_t)value;
    }
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    uint8_t       *d = dest;
    const uint8_t *s = src;
    while (n--)
    {
        *d++ = *s++;
    }
    return dest;
}