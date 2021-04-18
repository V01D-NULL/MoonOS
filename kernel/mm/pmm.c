#include "pmm.h"
#include "paging/pfa.h"
#include <stdint.h>

pmm_bitmap_t pmm_bitmap;
size_t highest_page;
static bitmap_size_type *bitmap; //Bitmap buffer / bitmap arena
liballoc_bitmap_t bitmap_manager;

extern const size_t __offset__;

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
    
    //Used for sanity check
    bitmap = (bitmap_size_type*) 0x0;

    //Set the size of the bitmap
    //highest_page / PAGE_SIZE = amount of pages in total, and higest_page / PAGE_SIZE / 8 will get the amount of bytes the bitmap will occupy since 1 byte = 8 bits
    size_t bitmap_size_bytes = ALIGN_UP(highest_page / PAGE_SIZE / 8);
    pmm_bitmap.size = bitmap_size_bytes;

    debug("no. of pages in total: %d\n", ALIGN_UP(highest_page / PAGE_SIZE));
    
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
            
            //Set the base of the bitmap ((size_t)&__offset__ makes sure that the bitmap is within the higher half of the kernel and not at mmap[i].base which would likely be in unmapped memory)
            bitmap = (bitmap_size_type*) (mmap[i].base + (size_t)&__offset__);

            //Create a bitmap
            bitmap_manager = bitmap_init(bitmap, bitmap_size_bytes);

            debug("Bitmap base: %p\n", bitmap);
            debug("Start of the bitmap: %llx, %d\n", mmap[i].base, mmap[i].base);

            // Set each entry to free
            memset((uint8_t*) bitmap_manager.pool, 0x0, bitmap_size_bytes);

            //Resize page
            mmap[i].base += bitmap_size_bytes;
            mmap[i].length -= bitmap_size_bytes;

            //Mark the the bitmap in the resized page as reserved. Moments like those show emphasize how badly I need a kernel panic implementation
            if (pfa_mark_page_as_used(bitmap_manager.pool, (void*)(mmap[i].base +  mmap[i].length))) {
                debug("Page already reserved!\nBitmap cannot be stored.\n");
                for (;;)
                    asm ("hlt");
            }

            debug("Shrunk mmap entry: %ld\nShrunk mmap base: %ld\n", mmap[i].base, mmap[i].length);
            break;
        }
    }

    //Sanity check, if this fails then there isn't enough memory to store the bitmap
    if (bitmap == (bitmap_size_type*)0x0) {
        kprintf("Son of a bit!\nYour computer doesn't have enough memory to store the bitmap!\n");
        for (;;)
            asm ("hlt");
    }

    //Step 3.
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
        {
        }
    }
    debug("mmap: 0x%llx\nhighest page: 0x%llx\n", (uint64_t) bitmap, highest_page);
    
    debug("\n");
    debug("%d\n", entries);
    
    bitmap_log_all_bits(bitmap_manager);

    //Step 4.
    //TODO: Mark kernel (end - start) as reserved
}
