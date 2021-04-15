/**
 * @file bitmap.c
 * @author Tim (V01D)
 * @brief 
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "bitmap.h"
#include "drivers/vga/vga.h"

static bitmap_size_type get(bitmap_size_type byte, uint8_t bit);
void bitmap_set(bitmap_size_type *bitmap, uint8_t bit);
void bitmap_unset(bitmap_size_type *bitmap, uint8_t bit);
bitmap_size_type bitmap_get(bitmap_size_type *bitmap, uint8_t bit);
bitmap_size_type bitmap_allocate();

/**
* @brief Set all bits in the bitmap arena to 0 (free)
* @param[in] bitmap A handle to the bitmap that should be accessed
*/
void bitmap_purge(liballoc_bitmap_t bitmap)
{
    for (int i = 0; i < bitmap.size; i++)
    {
        bitmap.clear(bitmap.pool, i);
    }
}

/**
* @brief Set all bits in the bitmap arena to 1 (used)
* @param[in] bitmap A handle to the bitmap that should be accessed
*/
void bitmap_fill(liballoc_bitmap_t bitmap)
{
    for (int i = 0; i < bitmap.size; i++)
    {
        bitmap.set(bitmap.pool, i);
    }
}

/**
* @brief Log each bit in the bitmap
* @param[in] bitmap A handle to the bitmap that should be accessed
*/
void bitmap_log_all_bits(liballoc_bitmap_t bitmap)
{
    for (int i = 0; i < bitmap.size; i++)
    {
        debug("%ld", bitmap.get(bitmap.pool, i));
    }
    debug("\n");
}

/**
* @brief Create a bitmap
* @param[in] pool  A pointer to the bitmap arena
* @param[in] size  The size of the bitmap. i.e. the amount of bits in the bitmap
*/
liballoc_bitmap_t bitmap_init(bitmap_size_type *pool, uint64_t size)
{
    liballoc_bitmap_t bmp;
    bmp.size  = size;
    bmp.pool  = pool;
    bmp.clear = bitmap_unset;
    bmp.set   = bitmap_set;
    bmp.alloc = bitmap_allocate;
    bmp.get   = bitmap_get;
    return bmp;
}

/**
* @brief Mark the first free block of memory in the bitmap arena as used (1)
* @param[in] bitmap A pointer to the bitmap arena
* @param[in] bit    The bit offset which should be modified
*/
bitmap_size_type bitmap_allocate()
{
    //TODO: Allocate the first free block and return a pointer to the allocated memory
    return 0;
}

/**
* @brief Set a single bit in the bitmap
* @param[in] bitmap A pointer to the bitmap arena
* @param[in] bit    The bit offset which should be modified
*/
void bitmap_set(bitmap_size_type *bitmap, uint8_t bit)
{
    bitmap[bit / BITMAP_BLOCK_SIZE] |= (1 << (bit % BITMAP_BLOCK_SIZE));
    debug("bitmap::liballoc::set(): Set bit: %d\n", bit);
}

/**
* @brief Clear a single bit in the bitmap
* @param[in] bitmap A pointer to the bitmap arena
* @param[in] bit    The bit offset which should be modified
*/
void bitmap_unset(bitmap_size_type *bitmap, uint8_t bit)
{
    bitmap[bit / BITMAP_BLOCK_SIZE] &= ~(1 << (bit % BITMAP_BLOCK_SIZE));
    debug("bitmap::liballoc::clear(): Cleared bit: %d\n", bit);
}

/**
* @brief Get a single bit in the bitmap
* @param[in] bitmap A pointer to the bitmap arena
* @param[in] bit    The bit offset which should be modified
*/
bitmap_size_type bitmap_get(bitmap_size_type *bitmap, uint8_t bit)
{
    return get(bitmap[bit / BITMAP_BLOCK_SIZE], bit % BITMAP_BLOCK_SIZE);
}

/**
* @brief Internal function (not intended for use by anything except the bitmap.c source file)
*/
static bitmap_size_type get(bitmap_size_type byte, uint8_t bit)
{
    return (byte >> bit) & 1;
}