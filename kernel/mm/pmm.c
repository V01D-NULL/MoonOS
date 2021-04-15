#include "pmm.h"
#include <stdint.h>

pmm_bitmap_t pmm_bitmap;
uint64_t highest_page;

/**
 * @brief The bitmap of the physical memory manager
 * 
 */
uint8_t *bitmap = NULL; //Bitmap buffer / bitmap arena

#define DIV_ROUND(x, y) ((x + (y - 1)) / y)

void init_pmm(struct stivale2_struct_tag_memmap *mmap, int entries)
{
    //take the first memmap entry, and if it's big enough set its start as the physical address of the bitmap.
    //increment the address of that entry by the size used

    /*
        TODO:
            - Integrate liballoc's bitmap into the pmm
    */

    //Calculate the size of the bitmap 
    for (int i = 0; i < mmap->entries; i++)
    {
        struct stivale2_mmap_entry *local_mmap = &mmap->memmap[i];

        if (local_mmap->type != STIVALE2_MMAP_USABLE)
            continue;

        pmm_bitmap.top = local_mmap->base + local_mmap->length;

        if (pmm_bitmap.top > highest_page)
        {
            highest_page = pmm_bitmap.top;
        }
    }

    uint64_t memory_size = DIV_ROUND(highest_page, PAGE_SIZE * BLOCK_SZ);
	uint64_t bitmap_size = memory_size / 8;
    pmm_bitmap.size = bitmap_size;

    debug("Bitmap top  address: 0x%x\n", pmm_bitmap.top);
    debug("bitmap size: 0x%x, %ld\n", bitmap_size, bitmap_size);

    //Find a big enough block to host the bitmap
    for (int i = 0; i < mmap->entries; i++)
    {
        struct stivale2_mmap_entry *local_mmap = &mmap->memmap[i];

        if (local_mmap->type != STIVALE2_MMAP_USABLE)
            continue;

        //We found a large enough block of memory to host the bitmap!
        if (local_mmap->length > bitmap_size)
        {
            debug("Found a big enough block of  memory to host the bitmap (size: %ld), bitmap_size: %ld\n", local_mmap->length, bitmap_size);
            debug("%p\n", bitmap);
            for (int i = 0; i < 100; i++)
            {
                bitmap[i] = 1;
                debug("bitmap: %p | i: %d\n", bitmap, i);
            }
            // memset((uint8_t*)bitmap, 0xF, bitmap_size);
            debug("bitmap: %p\n", bitmap);
            
            break;
        }
    }
}