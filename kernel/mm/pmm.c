#include "pmm.h"
#include "paging/pfa.h"
#include <stdint.h>
#include "../drivers/gfx/gfx.h"

size_t highest_page;
pmm_t pmm;

void pmm_init(struct stivale2_mmap_entry *mmap, int entries)
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
    
    debug("Bitmap top  address: 0x%lx\n", top);
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
            
            //Set the base of the bitmap (MM_BASE makes sure that the bitmap is within the higher half of the kernel and not at mmap[i].base which would likely be in unmapped memory)
            pmm.bitmap = (bitmap_size_type*) (mmap[i].base + MM_BASE);

            //Create a bitmap
            pmm.bitmap_manager = bitmap_init(pmm.bitmap, bitmap_size_bytes);

            debug("Bitmap base: %p\n", pmm.bitmap);
            debug("Start of the bitmap: %llx, %d\n", mmap[i].base, mmap[i].base);

            // Set each entry to free
            memset((uint8_t*) pmm.bitmap_manager.pool, 0x0, bitmap_size_bytes);

            //Resize page
            mmap[i].base += bitmap_size_bytes;
            mmap[i].length -= bitmap_size_bytes;
            pfa_mark_page_as_used((void*)(mmap[i].base +  mmap[i].length), true);

            debug("Shrunk mmap entry: %ld\nShrunk mmap base: %ld\n", mmap[i].base, mmap[i].length);
            break;
        }
    }

    //Sanity check, if this fails then there isn't enough memory to store the bitmap
    if (pmm.bitmap == (bitmap_size_type*)0x0) {
        printk("ERR","Couldn't store bitmap\n");
        for (;;)
            asm ("hlt");
    }

    //Step 3.
    for (int i = 0; i < entries; i++)
    {
        if (mmap[i].type != STIVALE2_MMAP_USABLE || mmap[i].type != STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE)
        {
            if (mmap[i].type == STIVALE2_MMAP_KERNEL_AND_MODULES)
            {
                pfa_mark_page_as_used((void*) ((mmap[i].base + mmap[i].length) / PAGE_SIZE), true);
                debug("Reserved kernel and kernel modules\n");
            }
            pfa_mark_page_as_used((void*) ((mmap[i].base + mmap[i].length) / PAGE_SIZE), true);
        }
    }
    debug("Bitmap resides at: 0x%llx\nhighest page: 0x%llx\n", (uint64_t) pmm.bitmap, highest_page);

    printk("pmm", "Initialised pmm\n");
}

/**
 * @brief Alloc n number of pages & return a pointer to the allocated memory
 * 
 * @return void* Returns NULL on error and a pointer to the allocated memory on success.
 */
void *pmm_alloc()
{
    size_t offset = find_first_free_block();

    //Coffee is yuck, and so is c0ffee. Sympathy +1 (in my book) for pmm_alloc which thinks the same :-)
    if (offset == PMM_INVALID)
        return NULL;

    pfa_mark_page_as_used((void*)offset, false);
    return ((void*) BIT_TO_ADDRESS(offset));
}

//TODO: Add a pmm_alloc_pages which allocates n pages (4k in size). This will be a wrapper function for pfa_request_pages()
/**
 * @brief Alloc `n' pages (only use for things that are not meant to be de-allocated)
 * 
 * @param n Number of pages to allocate
 */
void pmm_alloc_pages(uint64_t n)
{
    for (int i = 0; i < n; i++)
    {
        pmm_alloc();
    }
}

/**
 * @brief Free a bit in the bitmap / free a page (using an address returned by pmm_alloc)
 * 
 * @param page 
 * @return int32_t Returns PMM_INVALID on error and 0 on success
 */
int32_t pmm_free(void *page)
{
    uint64_t int_page = (uint64_t)page;
    if (int_page > MM_BASE)
    {
        int_page -= MM_BASE;
        
        //Is the page/bit already free?
        if (pfa_mark_page_as_free((void*) ADDRESS_TO_BIT(int_page),false) == 1)
        {
            debug("pmm_free: Invalid free on bit %d | Reason: Bit is already free\n", ADDRESS_TO_BIT(int_page));
            return PMM_INVALID;
        }
    }
    return 0;
}

/**
 * @brief Find the first free bit in the bitmap and return it's index
 * 
 * @return uint32_t Returns PMM_INVALID on error and the index of the bit in the bitmap on success
 */
int32_t find_first_free_block()
{
    uint32_t bit_offset = 0;

    for (int i = 0; i < pmm.bitmap_manager.size; i++)
    {
        bit_offset = pmm.bitmap_manager.get(pmm.bitmap_manager.pool, i) + i; //The .get() method will check if a bit at index 'i' is 1 or 0, so we add i to store the bit's index in the bitmap inside of offset, otherwise offset could only be 1 or 0 which isn't what I was aiming for

        //Bit is marked as used
        if (pmm.bitmap_manager.get(pmm.bitmap_manager.pool, bit_offset) == PMM_FREE)
        {
            return bit_offset;
        }
    }
    return PMM_INVALID;
}