#include "pfa.h"
#include "../ram.h"

extern const size_t kernel_end[];
extern liballoc_bitmap_t bitmap_manager;

//Mark a single page as free
int pfa_mark_page_as_free(uint8_t *bitmap, void *address)
{   
    uint64_t idx = (uint64_t) (uintptr_t) address / PAGE_SIZE;
    
    if (bitmap[idx] == BITMAP_USED)
    {
        // pmm_clear(idx);
        ram_manager_free(PAGE_SIZE);
        return 0;
    }
    return 1;
}

//Mark a single page as used
int pfa_mark_page_as_used(uint8_t *bitmap, void *address)
{
    uint64_t idx = (uint64_t)address / PAGE_SIZE;    

    /* Is this block already reserved? */
    if (bitmap_manager.get(bitmap_manager.pool, idx) == BITMAP_USED)
        return 1;

    debug("pfa_reserve_ok for address 0x%llx w/ bit index %ld\n", (uintptr_t)address, idx);
    
    /* Set bit in bitmap aka reserve a page */
    bitmap_manager.set(bitmap_manager.pool, idx);
    
    /* Update RAM stats */
    ram_manager_add(PAGE_SIZE);

    return 0;
}
