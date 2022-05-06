#ifndef BITMAP_H
#define BITMAP_H

#include <amd64/moon.h>
#include <ktypes.h>

#define BITMAP_BLOCK_SIZE sizeof(int) * 8


STATIC_INLINE void __set_bit(long *map, int bit)
{
	map[bit / BITMAP_BLOCK_SIZE] |= (1 << (bit % BITMAP_BLOCK_SIZE));
}

STATIC_INLINE void __clear_bit(long *map, int bit)
{
	map[bit / BITMAP_BLOCK_SIZE] &= ~(1 << (bit % BITMAP_BLOCK_SIZE));
}

STATIC_INLINE bool __check_bit(long *map, int bit)
{
	return (map[bit / BITMAP_BLOCK_SIZE] & (1 << (bit % BITMAP_BLOCK_SIZE)));
}

#endif // BITMAP_H