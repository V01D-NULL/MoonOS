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

// Returns 1 if src is less than dst
// Returns -1 if src is greater than dst
// Returns 0 if the length of src and dst are equal
uint64_t memcmp(const void *src, void *dst, uint64_t n)
{
    const uint8_t *src_a = src;
    uint8_t       *dst_a = dst;

    for (int i = 0; i < n; i++)
    {
        if (src_a[i] < dst_a[i])
            return 1;
        else if (src_a[i] > dst_a[i])
            return -1;
    }
    return 0;
}
