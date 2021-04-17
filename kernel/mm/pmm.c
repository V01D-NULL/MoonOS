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
        debug("S1: mmap[%d].type = %d\n", i, mmap[i].type);
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
    size_t bitmap_size_bytes = ALIGN_UP(highest_page / PAGE_SIZE / 8);
    
    //Create a bitmap
    liballoc_bitmap_t bitmap_manager = bitmap_init(bitmap, bitmap_size_bytes);

    debug("Bitmap top  address: 0x%x\n", pmm_bitmap.top);
    debug("bitmap size: 0x%x, %ld\n", bitmap_size_bytes, bitmap_size_bytes);

    //Step 2. Find a big enough block to host the bitmap
    for (int i = 0; i < entries; i++)
    {
        debug("S2: mmap[%d].type = %d\n", i, mmap[i].type);
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        //We found a large enough block of memory to host the bitmap!
        if (mmap[i].length >= bitmap_size_bytes)
        {
            debug("Found a big enough block of  memory to host the bitmap (size: %ld), bitmap_size_bytes: %ld\n", mmap[i].length, bitmap_size_bytes);
            debug("bitmap: %p\n", bitmap); //0000000000000000
            
            //BUG:
            //This sets all the bits to 1, but it also overwrites the mmap structure with 0xff, why?!
            memset((uint8_t*) bitmap, 0xff, bitmap_size_bytes);
            
            debug("bitmap: %p\n", bitmap); //0000000000000000

            //Resize page
            mmap[i].base += bitmap_size_bytes;
            mmap[i].length -= bitmap_size_bytes;
            
            // bitmap_log_all_bits(bitmap_manager); //Prints the bitmap (this shows that all bits are indeed set to 1)
            // debug("Shrunk mmap entry: %ld\nShrunk mmap base: %ld", mmap[i].base, mmap[i].length);
            break;
        }
    }

    //Step 3. 
    //This doesn't work because the memset above overwrites the values of the mmap struct, so the mmap.type will equal -1 for all checks, thus skipping this step entirely
    // for (int i = 0; i < entries; i++)
    // {
    //     debug("S3: mmap[%d].type = %d\n", i, mmap[i].type);

    //     if (mmap[i].type != STIVALE2_MMAP_USABLE) && mmap[i].type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE && mmap[i].type != STIVALE2_MMAP_KERNEL_AND_MODULES)
    //         continue;

    //     for (uintptr_t j = 0; j < mmap[i].length; j += PAGE_SIZE)
    //     {
    //         bitmap_manager.clear(bitmap, ALIGN_DOWN(mmap[i].base + j) / PAGE_SIZE);
    //     }
    // }
    // bitmap_log_all_bits(bitmap_manager);
}
