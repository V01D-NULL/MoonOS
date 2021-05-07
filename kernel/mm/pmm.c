#include "pmm.h"
#include "paging/pfa.h"
#include <stdint.h>
#include "../drivers/gfx/gfx.h"


size_t highest_page;
pmm_t pmm;

extern const size_t __offset__;

void init_pmm(struct stivale2_mmap_entry *mmap, int entries)
{
    size_t top = 0;
    //Step 1. Calculate the size of the bitmap 
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
            continue;

        top = mmap[i].base + mmap[i].length;

        if (top > highest_page)
        {
            highest_page = top;
        }
    }
    
    //Used for sanity check
    pmm.bitmap = (bitmap_size_type*) 0x0;

    //Set the size of the bitmap
    //highest_page / PAGE_SIZE = amount of pages in total, and higest_page / PAGE_SIZE / 8 will get the amount of bytes the bitmap will occupy since 1 byte = 8 bits
    size_t bitmap_size_bytes = ALIGN_UP(highest_page / PAGE_SIZE / 8);
    pmm.size = bitmap_size_bytes;

    debug("no. of pages in total: %d\n", ALIGN_UP(highest_page / PAGE_SIZE));
    
    debug("Bitmap top  address: 0x%x\n", top);
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
            pmm.bitmap = (bitmap_size_type*) (mmap[i].base + (size_t)&__offset__);

            //Create a bitmap
            pmm.bitmap_manager = bitmap_init(pmm.bitmap, bitmap_size_bytes);

            debug("Bitmap base: %p\n", pmm.bitmap);
            debug("Start of the bitmap: %llx, %d\n", mmap[i].base, mmap[i].base);

            // Set each entry to free
            memset((uint8_t*) pmm.bitmap_manager.pool, 0x0, bitmap_size_bytes);

            //Resize page
            mmap[i].base += bitmap_size_bytes;
            mmap[i].length -= bitmap_size_bytes;

            //Mark the the bitmap in the resized page as reserved. Moments like those show emphasize how badly I need a kernel panic implementation
            if (pfa_mark_page_as_used((void*)(mmap[i].base +  mmap[i].length))) {
                debug("Page already reserved!\nBitmap cannot be stored.\n");
                for (;;)
                    asm ("hlt");
            }

            debug("Shrunk mmap entry: %ld\nShrunk mmap base: %ld\n", mmap[i].base, mmap[i].length);
            break;
        }
    }

    //Sanity check, if this fails then there isn't enough memory to store the bitmap
    if (pmm.bitmap == (bitmap_size_type*)0x0) {
        printk("pmm","Son of a bit!\nYour computer doesn't have enough memory to store the bitmap!\n");
        for (;;)
            asm ("hlt");
    }

    //Step 3.
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE)
        {
            for (int j = 0; j < mmap[i].length; i += PAGE_SIZE)
            {
                pfa_mark_page_as_used((void*) ((mmap[i].base + mmap[i].length) / PAGE_SIZE));
            }
        }
    }
    debug("mmap: 0x%llx\nhighest page: 0x%llx\n", (uint64_t) pmm.bitmap, highest_page);
    
    debug("\n");
    debug("%d\n", entries);

    printk("pmm", "Initialised pmm\n");
}

/**
 * @brief Alloc n number of pages & return a pointer to the allocated memory
 * 
 * @return int32_t Returns 1 on error and the bits index in the bitmap on success. This return value will be used to free the page/clear the bit in the bitmap later on
 */
int32_t pmm_alloc()
{
    int32_t offset = PMM_INVALID;

    offset = find_first_free_block();
    pmm.bitmap_manager.set(pmm.bitmap_manager.pool, offset);
    printk("pmm", "Reserving bitmap bit: %d\n", offset);   

    //Coffee is yuck, and so is c0ffee. Sympathy +1 (in my book) for pmm_alloc which thinks the same :-)
    if (offset == PMM_INVALID)
        return 1;

    ram_manager_add(PAGE_SIZE);

    return offset;
}

//TODO: Add a pmm_alloc_pages which allocates n pages (4k in size)
/**
 * @brief Free a bit in the bitmap / free a page
 * 
 * @param bit_index 
 * @return int32_t Returns 1 on error and 0 on success
 */
int32_t pmm_free(uint8_t bit_index)
{
    //Is the page/bit already free?
    if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, bit_index) == PMM_FREE)
    {
        return 1;
    }
    pmm.bitmap_manager.clear(pmm.bitmap_manager.pool, bit_index);
    printk("pmm", "Free'd bitmap bit %d\n", bit_index);
    ram_manager_free(PAGE_SIZE);
    return 0;
}

/**
 * @brief Find the first free bit in the bitmap and return it's index
 * 
 * @return uint32_t Returns 1 on error and the index of the bit in the bitmap on success
 */
int32_t find_first_free_block()
{
    uint32_t bit_offset = 0;

    for (int i = 0; i < pmm.bitmap_manager.size; i++)
    {
        bit_offset = pmm.bitmap_manager.get(pmm.bitmap_manager.pool, i);
        
        //Bit is marked as used
        if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, bit_offset) == PMM_FREE)
        {
            return bit_offset;
        }
    }
    return 1;
}