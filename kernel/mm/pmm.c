#include "pmm.h"
#include <stdint.h>

pmm_bitmap_t pmm_bitmap;
uint8_t *bitmap; //Bitmap buffer / bitmap arena
uint64_t highest_page;

#define DIV_ROUND(x, y) ((x + (y - 1)) / y)

void init_pmm(struct stivale2_struct_tag_memmap *mmap, int entries)
{
    //take the first memmap entry, and if it's big enough set its start as the physical address of the bitmap.
    //increment the address of that entry by the size used

    debug("Bitmap size: %lld\n", BITMAP_SIZE);
    pmm_bitmap.size = BITMAP_SIZE;
    

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
    
    debug("Bitmap top  address: 0x%x\n", pmm_bitmap.top);
    debug("bitmap size: 0x%x, %ld\n", bitmap_size, bitmap_size);

    //Find a big enough block to host the bitmap
    for (int i = 0; i < mmap->entries; i++)
    {
        struct stivale2_mmap_entry *local_mmap = &mmap->memmap[i];

        if (local_mmap->type != STIVALE2_MMAP_USABLE)
            continue;

        //We found a large enough block of memory to host the bitmap!
        if (local_mmap->length >= bitmap_size)
        {
            debug("Found a big enough block of  memory to host the bitmap (size: %ld)\n", local_mmap);
            debug("bitmap: %p\n", bitmap);
            for (int j = 0; j < bitmap_size; j++)
            {
                bitmap[j] = PMM_USED; //does weird stuff to the video display (erases alot of stuff)
            }
            debug("\nbitmap: %p\n", bitmap);
            break;
        }
        
    }
}

void pmm_set(uint8_t bit)
{
    bitmap[bit / BLOCK_SZ] |= (1 << (bit % BLOCK_SZ));
}

void pmm_unset(uint8_t bit)
{
    bitmap[bit / BLOCK_SZ] &= ~(1 << (bit % BLOCK_SZ));
}
