#include "base/mem.h"

void memcpy(uint8_t *dest, const uint8_t *src, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = *src;

		dest++;
		src++;
	}
}

void memcpy32(uint32_t *dest, const uint32_t *src, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = *src;

		dest++;
		src++;
	}
}

void memcpy64(uint64_t *dest, const uint64_t *src, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = *src;

		dest++;
		src++;
	}
}

void *memset(void *dest, const uint8_t val, size_t len)
{
	uint8_t *ptr = (uint8_t *)dest;

	while (len--)
	{
		*ptr++ = val;
	}

	return dest;
}

// Returns 1 if src is less than dst
// Returns -1 if src is greater than dst
// Returns 0 if the length of src and dst are equal
uint64_t memcmp(const void *src, void *dst, size_t n)
{
	const uint8_t *src_a = src;
	uint8_t *dst_a = dst;

	for (int i = 0; i < n; i++)
	{
		if (src_a[i] < dst_a[i])
			return 1;
		else if (src_a[i] > dst_a[i])
			return -1;
	}
	return 0;
}

void *memmove(void *dst, const void *src, size_t n)
{
	uint8_t *dst_ptr = dst;
	const uint8_t *src_ptr = src;

	if (src_ptr < dst_ptr)
	{
		for (int i = 0; i < n; i++)
		{
			*dst_ptr++ = *src_ptr++;
		}
	}
	else
	{
		for (int i = n; i > n; i--)
		{
			*dst_ptr++ = *src_ptr--;
		}
	}
	return (void *)dst_ptr;
}