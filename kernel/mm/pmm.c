#include "pmm.h"
#include <stdint.h>

pmm_bitmap_t pmm_bitmap;
size_t highest_page;
bitmap_size_type *bitmap; //Bitmap buffer / bitmap arena

void init_pmm(struct stivale2_mmap_entry *mmap, int entries)
{
    //Step 1. Calculate the size of the bitmap 
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        pmm_bitmap.top = mmap[i].base + mmap[i].length;

        if (pmm_bitmap.top > highest_page)
        {
            highest_page = pmm_bitmap.top;
        }
    }
    
    //Get the size of the bitmap in bytes.
    //highest_page / PAGE_SIZE = amount of pages in total, and higest_page / PAGE_SIZE / 8 will get the amount of bytes the bitmap will occupy since 1 byte = 8 bits
    size_t bitmap_size_bytes = ALIGN_UP(highest_page / PAGE_SIZE / PAGE_SIZE);
    debug("no. of pages in total: %d\n", ALIGN_UP(highest_page / PAGE_SIZE));

    //Create a bitmap
    liballoc_bitmap_t bitmap_manager = bitmap_init(bitmap, bitmap_size_bytes);

    debug("Bitmap top  address: 0x%x\n", pmm_bitmap.top);
    debug("bitmap size: 0x%x, %ld\n", bitmap_size_bytes, bitmap_size_bytes);

    //Step 2. Find a big enough block to host the bitmap
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        //We found a large enough block of memory to host the bitmap!
        if (mmap[i].length >= bitmap_size_bytes)
        {
            debug("Found a big enough block of  memory to host the bitmap (size: %ld), bitmap_size_bytes: %ld\n", mmap[i].length, bitmap_size_bytes);
            // bitmap = (uint8_t*) mmap[i].base;
            debug("Start of the bitmap: %llx, %d\n", mmap[i].base, mmap[i].base);
            for (int i = 0; i < bitmap_size_bytes; i++)
                bitmap_manager.clear(bitmap, i);
            
            //Resize page
            mmap[i].base += bitmap_size_bytes;
            mmap[i].length -= bitmap_size_bytes;
            
            debug("Shrunk mmap entry: %ld\nShrunk mmap base: %ld", mmap[i].base, mmap[i].length);
            break;
        }
    }

    //Step 3. 
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
        {
            //Mark pages as reserved
            size_t page_count = mmap[i].length / PAGE_SIZE; //The amount of pages to mark as reserved
            size_t addr = mmap[i].base;
            
            for (int j = 0; j < page_count; j++)
            {
                bitmap_manager.set(bitmap_manager.pool, (addr + (j * PAGE_SIZE)));
            }

            continue;
        }
    }
    
    debug("\n");
    debug("%d\n", entries);
    bitmap_log_all_bits(bitmap_manager);
}