#ifndef BITMAP_H
#define BITMAP_H

#include <base/base-types.h>

#define BITMAP_BLOCK_SIZE sizeof(int) * 8


inline void __set_bit(long *map, int bit)
{
	map[bit / BITMAP_BLOCK_SIZE] |= (1 << (bit % BITMAP_BLOCK_SIZE));
}

inline void __clear_bit(long *map, int bit)
{
	map[bit / BITMAP_BLOCK_SIZE] &= ~(1 << (bit % BITMAP_BLOCK_SIZE));
}

inline bool __check_bit(long *map, int bit)
{
	return (map[bit / BITMAP_BLOCK_SIZE] & (1 << (bit % BITMAP_BLOCK_SIZE)));
}

#endif // BITMAP_H