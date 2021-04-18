/**
 * @file bitmap.h
 * @author Tim (V01D)
 * @brief 
 * @version 0.2
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 * Create and manage multiple bitmaps
 */

#ifndef LIB_ALLOC_BITMAP_H
#define LIB_ALLOC_BITMAP_H

#include <stdint.h>

/* bitmap size can be changed globally by changing this typedef */
typedef uint32_t bitmap_size_type;

//BITMAP_BLOCK_SIZE = bitmap_size_type in bits
#define BITMAP_BLOCK_SIZE       (sizeof(bitmap_size_type) * 8)
#define BITMAP_USED    1
#define BITMAP_FREE    0

/* main functions */
typedef struct liballoc_bmp {
    bitmap_size_type   *pool; //also known as "arena" or "bitmap"
    uint64_t           size;
    void              (*set)   (bitmap_size_type*, bitmap_size_type);
    bitmap_size_type  (*get)   (bitmap_size_type*, bitmap_size_type);
    void              (*clear) (bitmap_size_type*, bitmap_size_type);
    bitmap_size_type  (*alloc) ();
} liballoc_bitmap_t;

/* init */
liballoc_bitmap_t bitmap_init(bitmap_size_type *pool, uint64_t size);

/* misc */
void bitmap_log_all_bits(liballoc_bitmap_t bitmap);
void bitmap_purge(liballoc_bitmap_t bitmap);
void bitmap_fill(liballoc_bitmap_t bitmap);

#endif // LIB_ALLOC_BITMAP_H
